/**
 * @file simulation.cpp
 * @brief Demonstration of the Lock-Free Single-Producer Single-Consumer (SPSC) Architecture.
 * @details
 * This simulation mimics a high-frequency trading server environment:
 * 1. Network Thread (Producer): Represents the NIC (Network Interface Card) receiving packets.
 * 2. Engine Thread (Consumer): Represents the dedicated core processing the Order Book.
 * 3. LockFreeQueue (Ring Buffer): The "lockless bridge" transferring data between threads.
 *
 * * Key Takeaway: The Matching Engine runs at 100% speed without ever waiting for a mutex.
 */
#include <iostream>
#include <thread>
#include "LOB/OrderBook.h"
#include "LOB/LockFreeQueue.h"

// A struct representing a raw packet from the exchange.
// In a real system, this would be a specialized network packet (e.g., UDP/TCP payload).
struct OrderRequest {
    LOB::OrderId id;
    LOB::Price price;
    LOB::Quantity qty;
    LOB::Side side;
    bool isCancel;
};

/**
 * @brief The Producer Thread (Simulates Network Hardware)
 * @details
 * Generates orders as fast as possible and pushes them into the Ring Buffer.
 * If the buffer is full, it "busy waits" (spins) until space is available.
 */
void networkThread(LOB::LockFreeQueue<OrderRequest>& queue){
    std::cout << "[Network] Started. Generating 500,000 orders...\n";

    for(int i=0; i<500000; ++i){
        OrderRequest req = {
            (uint64_t)i,                  // Unique ID
            100 + (uint64_t)(i % 10),     // Price 100-109
            10,                           // Quantity
            (i % 2 == 0 ? LOB::Side::Buy : LOB::Side::Sell), // Alternate Buy/Sell
            false                         // Not a cancellation
        };

        // Busy Wait Strategy:
        // We do not sleep if the queue is full. We spin until the Engine catches up.
        // This avoids the latency penalty of context switching.
        while(!queue.push(req)){
            // In a real Low-Latency setup, we might use _mm_pause() here.
        }
    }
    std::cout << "[Network] DONE. All orders pushed.\n";
}

/**
 * @brief The Consumer Thread (The Matching Engine)
 * @details
 * Polls the Ring Buffer for new messages.
 * Note: This thread owns the OrderBook exclusively, so it needs NO locks for the book itself.
 */
void engineThread(LOB::LockFreeQueue<OrderRequest>& queue, LOB::OrderBook& book){
    std::cout << "[Engine] Started. Waiting for data...\n";
    
    int processedCount = 0;
    while(processedCount < 500000){
        OrderRequest req;
        
        // Non-blocking pop. If false, the queue is empty.
        if(queue.pop(req)){
            if(req.isCancel){
                book.cancelOrder(req.id);
            }
            else{
                book.addOrder(req.id, req.price, req.qty, req.side);
            }
            processedCount++;
        }
        // If queue is empty, we just loop again immediately (Busy Wait).
        // The CPU core is dedicated 100% to this while loop.
    }
    std::cout << "[Engine] DONE. Processed " << processedCount << " orders.\n";
}

int main(){
    std::cout << "--- LOCK-FREE ARCHITECTURE DEMO ---\n";

    // 1. Initialize the Ring Buffer (Size 1024)
    // Acts as the buffer between Network Card and CPU.
    LOB::LockFreeQueue<OrderRequest> queue(1024);
    
    // 2. Initialize the Engine
    // Note: We use the standard 'OrderBook', NOT 'ThreadSafeOrderBook'.
    // The Lock-Free Queue guarantees that only one thread accesses the engine at a time.
    LOB::OrderBook book;

    // 3. Launch Threads (Pinning to cores would happen here in production)
    std::thread producer(networkThread, std::ref(queue));
    std::thread consumer(engineThread, std::ref(queue), std::ref(book));

    // 4. Wait for completion
    producer.join();
    consumer.join();

    std::cout << "--- SIMULATION COMPLETE ---\n";
    return 0;
}