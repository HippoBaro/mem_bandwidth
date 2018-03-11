#include <numeric>
#include <iostream>
#include <iomanip>
#include <ratio>
#include <array>

#include "kernels.hpp"
#include "schedulers.hpp"

template<size_t RoundCount, typename Scheduler>
auto make_stats(Scheduler &scheduler) {
    auto bandwidths = std::array<decltype(std::declval<Scheduler>()()), RoundCount>();
    std::generate(bandwidths.begin(), bandwidths.end(), [&scheduler] { return scheduler(); });
    auto minmax = std::minmax_element(bandwidths.begin() + 1, bandwidths.end());
    return std::make_tuple(*minmax.second, *minmax.first, std::accumulate(bandwidths.begin() + 1, bandwidths.end(), 0.0) / RoundCount);
}

int format(std::tuple<double, double, double> const& result, std::string const& kernel_name) {
    std::cout << std::setw(8)  << std::left  << kernel_name
              << std::setw(13) << std::right << std::fixed << std::setprecision(8) << std::get<0>(result)
              << std::setw(13) << std::right << std::fixed << std::setprecision(8) << std::get<1>(result)
              << std::setw(13) << std::right << std::fixed << std::setprecision(8) << std::get<2>(result) << std::endl;
    return 0;
}

template<typename Scheduler, size_t Mem, template<typename, typename > typename ...Kernels>
void benchmark() {
    std::cout << "\nTesting using " << Scheduler::name << ":\n";
    std::cout << std::setw(8)  << std::left  << "Kernel" << std::setw(12) << std::right << "Max (GB/s)"
              << std::setw(13) << std::right << "Min (GB/s)" << std::setw(12) << std::right << "Avr (GB/s)" << std::endl;

    using expander = int[];
    (void) expander{ 0, (format(make_stats<10>(time_execution<Scheduler, double, std::mega::num * Mem, Kernels>), Kernels<int*, int>::name))... };
}


int main() {
    pool = std::make_unique<thread_pool>(std::thread::hardware_concurrency());

    benchmark<serial_scheduler, 100, fill, copy, scale, sum, triad, vsum, vprod>();
    benchmark<parallel_scheduler, 50, fill, copy, scale, sum, triad, vsum, vprod>();
    return EXIT_SUCCESS;
}
