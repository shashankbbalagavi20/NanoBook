cmake_minimum_required(VERSION 3.15)
project(NanoBook VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Point to our code
include_directories(include)

# Create the executable "NanoBook" from "src/main.cpp"
add_executable(NanoBook src/main.cpp)