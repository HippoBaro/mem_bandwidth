cmake_minimum_required(VERSION 3.0)
project(mem_bandwidth)

set(CMAKE_CXX_STANDARD 17)
set(OPTS -Wall -Wextra -Werror -march=native -O3)

add_executable(mem_bandwidth main.cpp kernels.hpp schedulers.hpp threadpool.hpp safequeue.hpp)

target_compile_options(mem_bandwidth PUBLIC ${OPTS})