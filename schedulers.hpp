//
// Created by Hippolyte Barraud on 10/03/2018.
//

#ifndef MEM_BANDWIDTH_SCHEDULERS_HPP
#define MEM_BANDWIDTH_SCHEDULERS_HPP

#include <memory>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <future>
#include <vector>
#include <iostream>
#include <random>

#include "threadpool.hpp"

std::unique_ptr<thread_pool> pool;

struct serial_scheduler {
    static constexpr auto name = "Single threaded serial scheduler";

    struct allocator {
        template<typename Vec>
        static auto alloc() {
            return std::make_unique<std::tuple<Vec, Vec, Vec>>();
        }
    };

    template<typename TNum, typename Vec, template<class, class> typename Kernel, typename Arg = decltype(allocator::template alloc<Vec>())>
    static inline auto schedule(Arg &arg) {
        auto start = std::chrono::high_resolution_clock::now();
        Kernel<Vec, TNum>::run(std::get<0>(*arg), std::get<1>(*arg), std::get<2>(*arg));
        return std::chrono::high_resolution_clock::now() - start;
    }

    template<class TNum, size_t ElementCount, template<class, class> typename Kernel, typename Vec>
    static double result(int64_t delta) {
        return static_cast<double>((ElementCount) * Kernel<Vec, TNum>::bytes_per_iter) / delta;
    }
};

struct parallel_scheduler {
    static constexpr auto name = "Multi threaded parallel scheduler";

    struct allocator {
        template<typename Vec>
        static auto alloc() {
            std::vector<std::unique_ptr<std::tuple<Vec, Vec, Vec>>> ret(std::thread::hardware_concurrency());
            std::generate(ret.begin(), ret.end(), []() { return std::make_unique<std::tuple<Vec, Vec, Vec>>(); });
            return ret;
        }
    };

    template<typename TNum, typename Vec, template<class, class> typename Kernel, typename Arg = decltype(allocator::template alloc<Vec>())>
    static inline auto schedule(Arg &arg) {
        std::vector<std::future<std::chrono::nanoseconds>> res(std::thread::hardware_concurrency());
        std::vector<std::chrono::nanoseconds> resval(std::thread::hardware_concurrency());
        std::transform(arg.begin(), arg.end(), res.begin(), [] (auto &ptr) {
            return pool->submit([ &ptr]() { return serial_scheduler::template schedule<TNum, Vec, Kernel>(ptr); });
        });
        std::transform(res.begin(), res.end(), resval.begin(), [](auto &future) { return future.get(); });
        return *std::max_element(resval.begin(), resval.end());
    }

    template<class TNum, size_t ElementCount, template<class, class> typename Kernel, typename Vec>
    static double result(int64_t delta) {
        return static_cast<double>((ElementCount * std::thread::hardware_concurrency())  * Kernel<Vec, TNum>::bytes_per_iter) / delta;
    }
};

template<typename Scheduler, class TNum, size_t Size, template<class, class> typename Kernel, size_t ElementCount = Size / sizeof(TNum) / 3,
        typename Vec = typename std::array<TNum, ElementCount>>
double time_execution() {
    static thread_local auto arg = Scheduler::allocator::template alloc<Vec>();
    auto delta = Scheduler::template schedule<TNum, Vec, Kernel>(arg);
    return Scheduler::template result<TNum, ElementCount, Kernel, Vec>(delta.count());
}

#endif //MEM_BANDWIDTH_SCHEDULERS_HPP
