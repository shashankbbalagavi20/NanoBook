#include <iostream>
#include <thread>
#include <vector>
#include "LOB/ThreadSafeOrderBook.h"

void traderThread(LOB::ThreadSafeOrderBook& book, int id, int count) {
    for (int i = 0; i < count; ++i) {
        // Create unique Order IDs based on thread ID
        // Thread 1: 1000, 1001... Thread 2: 2000, 2001...
        LOB::OrderId orderId = (id * 10000) + i;
        
        // Alternate between Buy and Sell to force matches
        LOB::Side side = (i % 2 == 0) ? LOB::Side::Buy : LOB::Side::Sell;
        LOB::Price price = 100 + (i % 5); // Prices 100-104

        book.addOrder(orderId, price, 10, side);
    }
}

int main() {
    LOB::ThreadSafeOrderBook book;
    
    std::cout << "--- MULTI-THREADED STRESS TEST ---\n";
    std::cout << "Launching 4 threads firing 10,000 orders each...\n";

    std::vector<std::thread> threads;
    
    // Spawn 4 Threads
    for (int i = 1; i <= 4; ++i) {
        threads.emplace_back(traderThread, std::ref(book), i, 10000);
    }

    // Wait for them to finish
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "--- ALL THREADS FINISHED ---\n";
    std::cout << "Final Book State:\n";
    book.printBook();

    return 0;
}