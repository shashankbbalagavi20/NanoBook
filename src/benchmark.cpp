#include <benchmark/benchmark.h>
#include "LOB/Order.h"
#include "LOB/ObjectPool.h"

// Test 1: The slow way
static void BM_HeapAllocation(benchmark::State& state){
    for(auto _ : state){
        LOB::Order* o = new LOB::Order(1,100,10, LOB::Side::Buy);
        benchmark::DoNotOptimize(o);
        delete o;
    }
}
BENCHMARK(BM_HeapAllocation);

static void BM_ObjectPool(benchmark::State& state){
    LOB::ObjectPool<LOB::Order> pool(10000);
    for(auto _ : state){
        LOB::Order* o = pool.allocate(1, 100, 10, LOB::Side::Buy);
        benchmark::DoNotOptimize(o);
        pool.deallocate(o);
    }
}
BENCHMARK(BM_ObjectPool);

BENCHMARK_MAIN();