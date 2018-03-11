//
// Created by Hippolyte Barraud on 10/03/2018.
//

#ifndef MEM_BANDWIDTH_KERNELS_HPP
#define MEM_BANDWIDTH_KERNELS_HPP

#include <functional>
#include <algorithm>

template<typename T, int bytes, int flots>
struct kernel {
    static constexpr auto bytes_per_iter = bytes * sizeof(T);
};

template<typename Container, typename T>
struct fill : public kernel<T, 1, 0> {
    static constexpr auto name = "Fill";
    static inline void run(Container & first, Container const&, Container const&) {
        std::fill(first.begin(), first.end(), 3.14);
    }
};

template<typename Container, typename T>
struct copy : public kernel<T, 2, 0> {
    static constexpr auto name = "Copy";
    static inline void run(Container & first, Container const& second, Container const&) {
        volatile __attribute__((unused)) auto f = std::copy(second.begin(), second.end(), first.begin());
    }
};

template<typename Container, typename T>
struct scale : public kernel<T, 2, 1> {
    static constexpr auto name = "Scale";
    static inline void run(Container & first, Container const& second, Container const&) {
        std::transform(second.data(), second.data() + second.size(), first.data(), [](auto x) { return 3.14 * x; });
    }
};

template<typename Container, typename T>
struct sum : public kernel<T, 3, 1> {
    static constexpr auto name = "Sum";
    static inline void run(Container & first, Container const& second, Container const& third) {
        std::transform(second.data(), second.data() + second.size(), third.data(), first.data(), std::plus<T>());
    }
};

template<typename Container, typename T>
struct triad : public kernel<T, 3, 2> {
    static constexpr auto name = "Triad";
    static inline void run(Container & first, Container const& second, Container const& third) {
        std::transform(second.data(), second.data() + second.size(), third.data(), first.data(), [](const auto& x, const auto& y) {
            return x + 3.14 * y;
        });
    }
};

template<typename Container, typename T>
struct vsum : public kernel<T, 1, 1> {
    static constexpr auto name = "Vsum";
    static inline void run(Container const&, Container const& second, Container const&) {
        volatile __attribute__((unused)) auto f = std::accumulate(second.begin(), second.end(), 1.0, std::plus<T>());
    }
};

template<typename Container, typename T>
struct vprod : public kernel<T, 1, 1> {
    static constexpr auto name = "Vprod";
    static inline void run(Container const&first, Container const&, Container const&) {
        volatile __attribute__((unused)) auto f = std::accumulate(first.begin(), first.end(), 1.0, std::multiplies<T>());
    }
};

#endif //MEM_BANDWIDTH_KERNELS_HPP
