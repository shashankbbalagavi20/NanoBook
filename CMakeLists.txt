cmake_minimum_required(VERSION 3.15)
project(NanoBook VERSION 3.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# --- DEPENDENCIES ---
include(FetchContent)

# 1. Google Benchmark
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.8.3
)
FetchContent_MakeAvailable(googlebenchmark)

# 2. Google Test
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing()

# --- SOURCES ---
set(ENGINE_SOURCES src/engine/OrderBook.cpp src/engine/LimitLevel.cpp)

# --- EXECUTABLES ---

# 1. Main Dashboard (The "Product")
add_executable(NanoBook src/main.cpp ${ENGINE_SOURCES})

# 2. Benchmarks (The "Speed Proof")
add_executable(NanoBenchmark src/benchmark.cpp)
target_link_libraries(NanoBenchmark benchmark::benchmark_main)

# 3. Lock-Free Sim (The "Architecture Proof")
add_executable(NanoSimulation src/simulation.cpp ${ENGINE_SOURCES})

# 4. Test Suite (The "Reliability Proof")
add_executable(NanoTests 
    tests/OrderBookTests.cpp 
    tests/StressTests.cpp 
    ${ENGINE_SOURCES}
)
target_link_libraries(NanoTests GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(NanoTests)