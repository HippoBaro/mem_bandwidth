#ifndef MEM_BANDWIDTH_THREADPOOL_HPP
#define MEM_BANDWIDTH_THREADPOOL_HPP

#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include "safequeue.hpp"

class thread_pool {
private:
    class worker {
    private:
        thread_pool *m_pool;
        size_t m_id;
    public:
        worker(thread_pool *pool, const size_t id)
                : m_pool(pool), m_id(id) {
        }

        void operator()() {
            std::function<void()> func;
            bool dequeued;
            while (!m_pool->m_shutdown) {
                {
                    std::unique_lock<std::mutex> lock(m_pool->m_conditional_mutex);
                    if (m_pool->m_queue.empty()) {
                        m_pool->m_conditional_lock.wait(lock);
                    }
                    dequeued = m_pool->m_queue.dequeue(func);
                }
                if (dequeued) {
                    func();
                }
            }
        }
    };

    bool m_shutdown;
    safe_queue<std::function<void()>> m_queue;
    std::vector<std::thread> m_threads;
    std::mutex m_conditional_mutex;
    std::condition_variable m_conditional_lock;
public:
    explicit thread_pool(const int n_threads)
            : m_shutdown(false), m_threads(std::vector<std::thread>(static_cast<size_t>(n_threads))) {
        for (size_t i = 0; i < m_threads.size(); ++i) {
            m_threads[i] = std::thread(worker(this, i));
        }
    }

    thread_pool(const thread_pool &) = delete;

    thread_pool(thread_pool &&) = delete;

    thread_pool &operator=(const thread_pool &) = delete;

    thread_pool &operator=(thread_pool &&) = delete;

    virtual ~thread_pool() {
        shutdown();
    }

    // Waits until threads finish their current task and shutdowns the pool
    void shutdown() {
        m_shutdown = true;
        m_conditional_lock.notify_all();

        for (auto &m_thread : m_threads) {
            if (m_thread.joinable()) {
                m_thread.join();
            }
        }
    }

    // Submit a function to be executed asynchronously by the pool
    template<typename F, typename...Args>
    auto submit(F &&f, Args &&... args) -> std::future<decltype(f(args...))> {
        // Create a function with bounded parameters ready to execute
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        // Encapsulate it into a shared ptr in order to be able to copy construct / assign
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        // Wrap packaged task into void function
        std::function<void()> wrapper_func = [task_ptr]() {
            (*task_ptr)();
        };

        // Enqueue generic wrapper function
        m_queue.enqueue(wrapper_func);

        // Wake up one thread if its waiting
        m_conditional_lock.notify_one();

        // Return future from promise
        return task_ptr->get_future();
    }
};

#endif //MEM_BANDWIDTH_THREADPOOL_HPP
