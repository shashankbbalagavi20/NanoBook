/**
 * @file StressTests.cpp
 * @brief Concurrency and Stability tests for the ThreadSafeOrderBook.
 * @details
 * This file performs "Hammer Tests" to verify the SpinLock implementation.
 * It spawns multiple threads that aggressively hammer the engine with orders.
 * * Pass Condition: The program does not Segfault or throw exceptions.
 * * Fail Condition: Memory corruption (Segfault) due to race conditions.
 */

#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include "../include/LOB/ThreadSafeOrderBook.h"

/**
 * @brief A worker function mimicking an active high-frequency trader.
 * @param book Reference to the thread-safe engine.
 * @param id Unique ID for this thread (used to generate unique Order IDs).
 * @param count Number of orders to submit.
 */
void traderThread(LOB::ThreadSafeOrderBook& book, int id, int count) {
    for (int i = 0; i < count; ++i) {
        // ID Partitioning:
        // Thread 0 uses IDs 0-19999
        // Thread 1 uses IDs 1000000-1019999
        // This ensures logically valid IDs while testing concurrent map access.
        LOB::OrderId orderId = (id * 1000000) + i;
        
        // Toggle between Buy and Sell to trigger matching logic
        LOB::Side side = (i % 2 == 0) ? LOB::Side::Buy : LOB::Side::Sell;
        
        // Randomize price slightly to create different LimitLevels
        LOB::Price price = 100 + (i % 5);
        
        book.addOrder(orderId, price, 10, side);
    }
}

/**
 * @test ConcurrencyTest.NoCrashesUnderLoad
 * @brief Verifies that the SpinLock correctly protects the OrderBook.
 * @details
 * Scenario: 4 Threads x 20,000 Orders = 80,000 Operations.
 * Since std::map is NOT thread-safe, running this without a lock would
 * cause an immediate Segmentation Fault (Crash).
 */
TEST(ConcurrencyTest, NoCrashesUnderLoad) {
    LOB::ThreadSafeOrderBook book;
    std::vector<std::thread> threads;
    
    // 4 threads is enough to saturate most CPU cores and force contention
    int numThreads = 4;
    int ordersPerThread = 20000;

    // The entire block is wrapped in EXPECT_NO_THROW.
    // If the SpinLock fails, the OS will throw a Segfault (Signal), 
    // causing the test runner to report a crash/failure.
    EXPECT_NO_THROW({
        // 1. Launch the fleet
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(traderThread, std::ref(book), i, ordersPerThread);
        }

        // 2. Wait for all to finish
        for (auto& t : threads) {
            t.join();
        }
    });
}