/**
 * @file benchmark.cpp
 * @brief Micro-benchmarks validating the performance gains of the custom memory allocator.
 * @details
 * This file uses Google Benchmark to compare:
 * 1. Standard Heap Allocation (new/delete) -> Involves syscalls and mutexes in malloc.
 * 2. Custom Object Pool (Slab Allocator) -> Involves only pointer arithmetic (LIFO).
 * * Expected Result: The ObjectPool should be 10x-50x faster than the heap.
 */
#include <benchmark/benchmark.h>
#include "LOB/Order.h"
#include "LOB/ObjectPool.h"

/**
 * @brief Benchmark 1: Standard C++ Heap Allocation
 * @details Tests the cost of 'new' and 'delete'.
 * - Performacne Hit 1: Finding a free block in the heap (Fragmented).
 * - Performance Hit 2: System calls (brk/mmap) if the heap needs to grow.
 * - Performance Hit 3: Thread-Safety locks inside malloc().
 */
static void BM_HeapAllocation(benchmark::State& state){
    for(auto _ : state){
        // The "Slow" Way
        LOB::Order* o = new LOB::Order(1,100,10, LOB::Side::Buy);
        
        // Prevent compiler from optimizing away the unused variable
        benchmark::DoNotOptimize(o);
        delete o;
    }
}
// Register the function as a benchmark
BENCHMARK(BM_HeapAllocation);

/**
 * @brief Benchmark 2: Custom Object Pool Allocation
 * @details Tests the cost of 'allocate' and 'deallocate' from tour pre-allocated vector.
 * - Performance Gain 1: O(1) pointer math (pop_back).
 * - Performance Gain 2: No System Calls (Zero Kernal crossings).
 * - Performance Gain 3: Hot Cache (LIFO strategy reuses the same cache line).
 */
static void BM_ObjectPool(benchmark::State& state){
    // Setup: Pre-allocate memory ONCE before the timer starts
    LOB::ObjectPool<LOB::Order> pool(10000);

    for(auto _ : state){
        // The "Fast" Way
        LOB::Order* o = pool.allocate(1, 100, 10, LOB::Side::Buy);

        benchmark::DoNotOptimize(o);
        
        pool.deallocate(o);
    }
}
BENCHMARK(BM_ObjectPool);

// Main function required by Google Benchmark
BENCHMARK_MAIN();