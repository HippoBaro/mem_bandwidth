#ifndef MEM_BANDWIDTH_SAFEQUEUE_HPP
#define MEM_BANDWIDTH_SAFEQUEUE_HPP

#include <mutex>
#include <queue>

// Thread safe implementation of a Queue using a std::queue
template<typename T>
class safe_queue {
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
public:
    safe_queue() = default;

    safe_queue(safe_queue &) {}

    ~safe_queue() = default;

    bool empty() {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    size_t size() {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    void enqueue(T &t) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(t);
    }

    bool dequeue(T &t) {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_queue.empty()) {
            return false;
        }
        t = std::move(m_queue.front());

        m_queue.pop();
        return true;
    }
};

#endif //MEM_BANDWIDTH_SAFEQUEUE_HPP
