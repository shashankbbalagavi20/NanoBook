/**
 * @file main.cpp
 * @brief NanoBook CLI - Live Trading Dashboard.
 * @details
 * This is the visual entry point for the project. It runs a continuous simulation
 * of market data and renders a "Bloomberg Terminal" style TUI (Text User Interface).
 * * NOTE: This is for demonstration purposes. For actual performance measurements,
 * see 'src/benchmark.cpp'.
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <iomanip>
#include "LOB/OrderBook.h"

/**
 * @brief Clears the console screen using ANSI escape codes.
 * @details
 * - \033[2J : Clears the entire screen.
 * - \033[1;1H : Moves the cursor to the top-left corner (Row 1, Col 1).
 * This creates a "static" dashboard effect instead of a scrolling log.
 */
void clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

/**
 * @brief Prints the static dashboard header.
 * @param orders Total number of orders processed in this session.
 */
void printHeader(int orders) {
    std::cout << "================================================================\n";
    std::cout << "   NANOBOOK v3.0  |  HFT ENGINE  |  LATENCY: <5ns  |  OPS: " << orders << "\n";
    std::cout << "================================================================\n";
    std::cout << "   [BID]                       [ASK]                \n";
    std::cout << "   Vol      Price  |  Price      Vol                \n";
    std::cout << "----------------------------------------------------------------\n";
}

int main() {
    // The core engine instance
    LOB::OrderBook book;
    
    // --- Random Number Generation Setup ---
    // We use std::mt19937 (Mersenne Twister) for high-quality random numbers
    // to simulate realistic market noise.
    std::mt19937 gen(std::random_device{}());
    
    // Price range: 98 to 102 (Tight spread simulation)
    std::uniform_int_distribution<> priceDist(98, 102);
    
    // Quantity range: Small retail orders (1) to large blocks (500)
    std::uniform_int_distribution<> qtyDist(1, 500);
    
    // Side: 0 = Buy, 1 = Sell
    std::uniform_int_distribution<> sideDist(0, 1);

    int ordersProcessed = 0;
    
    // --- Main Simulation Loop ---
    while (true) {
        // 1. Simulate a "Burst" of Market Activity
        // We process a batch of 10 orders before re-drawing the screen.
        for(int i=0; i<10; ++i) {
            LOB::Price p = priceDist(gen);
            LOB::Quantity q = qtyDist(gen);
            LOB::Side s = (sideDist(gen) == 0) ? LOB::Side::Buy : LOB::Side::Sell;
            
            // Static ID ensures unique Order IDs across iterations
            static uint64_t id = 1;
            
            // The Engine Call: This is where the magic happens (O(1) / O(log N))
            book.addOrder(id++, p, q, s);
            ordersProcessed++;
        }

        // 2. Render the TUI (Text User Interface)
        clearScreen();
        printHeader(ordersProcessed);
        
        // Print the top levels of the book
        book.printBook(); 
        
        std::cout << "\n----------------------------------------------------------------\n";
        std::cout << " System Status:  [ONLINE]  Matching Engine Active\n";
        std::cout << " Press Ctrl+C to Exit\n";

        // 3. Throttle the display
        // We sleep for 100ms so the human eye can actually read the numbers.
        // In a real HFT system, we would NEVER sleep!
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}