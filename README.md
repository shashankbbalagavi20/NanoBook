# NanoBook: High-Performance Limit Order Book

![Language](https://img.shields.io/badge/language-C%2B%2B20-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)

## Overview
**NanoBook** is a low-latency, cache-friendly Limit Order Book (LOB) matching engine implemented in modern C++ (C++20).

Designed for High-Frequency Trading (HFT) environments, this engine prioritizes **deterministic latency** and **hardware sympathy**. It avoids dynamic memory allocation on the hot path and utilizes custom memory management to ensure data locality.

## Key Features

### 🚀 Performance & Memory
* **Zero Hot-Path Allocations:** Implements a custom **Slab Allocator (Object Pool)** to eliminate `new`/`delete` calls during order processing, preventing heap fragmentation and syscall overhead.
* **Cache Friendliness:** Data structures are packed and aligned to cache lines (64 bytes) to minimize false sharing and L1/L2 cache misses.
* **Deterministic Execution:** Pre-allocated resources ensure predictable latency profiles under high load.

### ⚡ Data Structures
* **Intrusive Doubly Linked Lists:** Orders are stored using intrusive pointers within pre-allocated blocks, allowing **O(1)** order cancellation and execution without pointer chasing.
* **Price Level Management:** Sparse static arrays (or cache-optimized trees) for constant-time price level lookup.

### 🛠️ Tech Stack
* **Language:** C++20 (Concepts, Smart Pointers, Template Metaprogramming)
* **Build System:** CMake & Ninja
* **Testing:** GoogleTest (Unit Testing)
* **Benchmarking:** Google Benchmark (Micro-benchmarking latency)

## Architecture
The engine is composed of three distinct layers to separate mechanism from policy:
1.  **Core Engine:** The matching logic and order book state.
2.  **Memory Manager:** The pool allocator handling `Order` and `LimitLevel` lifecycles.
3.  **Gateway:** Interface for FIX/Binary protocols.

## Build Instructions

### Prerequisites
* C++20 compatible compiler (GCC 10+, Clang 12+, or MSVC 19.28+)
* CMake 3.15+

### Building the Project
```bash
# Clone the repository
git clone https://github.com/shashankbbalagavi20/NanoBook.git
cd NanoBook

# Configure and Build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Running tests
./build/unit_tests

# Running Benchmarks
./build/latency_benchmarks
```

## 🙌 Acknowledgements
Implemented from scratch in modern C++20 to deepen expertise in Systems Programming, OS Internals, and High-Performance Computing (HPC).