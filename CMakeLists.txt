cmake_minimum_required(VERSION 3.15)
project(NanoBook VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark testing" FORCE)
set(BENCHMARK_INSTALL_DOCS OFF CACHE BOOL "Disable benchmark docs" FORCE)

# Download Google Benchmark
include(FetchContent)
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.3
)
FetchContent_MakeAvailable(googlebenchmark)

add_executable(NanoBook src/main.cpp src/engine/LimitLevel.cpp src/engine/OrderBook.cpp)

add_executable(NanoBenchmark src/benchmark.cpp)
target_link_libraries(NanoBenchmark benchmark::benchmark_main)