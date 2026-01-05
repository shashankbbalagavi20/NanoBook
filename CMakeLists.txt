cmake_minimum_required(VERSION 3.15)
project(NanoBook VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(NanoBook src/main.cpp src/engine/LimitLevel.cpp src/engine/OrderBook.cpp)