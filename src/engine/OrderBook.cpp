/**
 * @file OrderBook.cpp
 * @brief Implementation of the core Matching Engine.
 * @details
 * This file contains the logic for:
 * 1. Order Injection (Adding orders to the book).
 * 2. Order Matching (Executing trades when prices cross).
 * 3. Order Cancellation (Removing orders efficiently).
 * 4. Memory Management (Using ObjectPool for zero-allocation runtime).
 */
#include "LOB/OrderBook.h"
#include <iostream>
#include <algorithm>

namespace LOB {

    // Initialize the memory pool with space for 100,000 orders to prevent runtime allocations.
    OrderBook::OrderBook() : orderPool_(10000) {}

    OrderBook::~OrderBook() {
        // Clean up all dynamically allocated LimitLevels
        for(auto& pair : bids_) delete pair.second;
        for(auto& pair : asks_) delete pair.second;
    }

    void OrderBook::addOrder(OrderId id, Price price, Quantity qty, Side side){
        // 1. Idemptency Check: Don't add duplicate IDs
        if(orderMap_.find(id) != orderMap_.end()){
            return;
        }

        // 2. Fast Allocation: Get a pre-allocated object from the pool (O(1))
        Order* order = orderPool_.allocate(id, price, qty, side);
        if(!order)
        {
            std::cerr << "[Error] Order Pool Exhausted!\n";
            return;
        }

        // 3. Indexing: Map ID -> Pointer for O(1) cancellation later
        orderMap_[id] = order;

        // 4. Update the Book: Add to the specific Price Level
        LimitLevel* level = getLimitLevel(price, side);
        level->append(order);

        // 5. Attempt Execution: Check if this new order crosses the spread
        match();
    }

    void OrderBook::printBook() const {
        std::cout << "\n--- ORDER BOOK SNAPSHOT ---\n";

        std::cout << "ASKS (Sellers):\n";
        // Iterate in reverse to show highest prices at the top
        for(auto it = asks_.rbegin(); it != asks_.rend(); ++it){
            std::cout << " Price: " << it->first << " | Vol: " << it->second->getVolume() << "\n";
        }

        std::cout << "-----------------------------\n";

        std::cout << "BIDS (Buyers):\n";
        for(auto const& [price, level] : bids_){
            std::cout << " Price: " << price << " | Vol: " << level->getVolume() << "\n";
        }

        std::cout << "-----------------------------\n";
    }

    LimitLevel* OrderBook::getLimitLevel(Price price, Side side){
        if(side == Side::Buy){
            if(bids_.find(price) == bids_.end()){
                bids_[price] = new LimitLevel(price);
            }
            return bids_[price];
        }
        else{
            if(asks_.find(price) == asks_.end()){
                asks_[price] = new LimitLevel(price);
            }
            return asks_[price];
        }
    }

    void OrderBook::match(){
        while (true){
            // Stop if one side is empty
            if(bids_.empty() || asks_.empty()){
                break;
            }

            // Get the best prices (Top of Book)
            auto bestBidLevel = bids_.begin()->second;
            auto bestAskLevel = asks_.begin()->second;

            // Spread Check: If Bid < Ask, no trade is possible.
            if(bestBidLevel->getPrice() < bestAskLevel->getPrice()){
                break;
            }

            // Get the oldest orders (FIFO) at these levels
            Order* bidOrder = bestBidLevel->getHead();
            Order* askOrder = bestAskLevel->getHead();

            // Calculate fill quantity (min of both)
            Quantity quantity = std::min(bidOrder->quantity, askOrder->quantity);

            std::cout << ">>> TRADE EXECUTE: " << quantity << " shares @ " << bestAskLevel->getPrice() << " (Bid #" << bidOrder->id << " vs Ask #" << askOrder->id << ")\n";

            // Update quantities
            bidOrder->quantity -= quantity;
            askOrder->quantity -= quantity;

            // Cleanup: If Bid is fully filled
            if(bidOrder->quantity == 0){
                bestBidLevel->remove(bidOrder);  // Unlink from list
                orderMap_.erase(bidOrder->id);   // Remove from index
                orderPool_.deallocate(bidOrder); // Return to pool

                // If the entire price level is empty, remove it
                if(bestBidLevel->isEmpty()){
                    delete bestBidLevel;
                    bids_.erase(bids_.begin()); // Iterator invalidation safe because we restart loop
                }   
            }

            // Cleanup: If Ask if fully filled
            if(askOrder->quantity == 0){
                bestAskLevel->remove(askOrder);
                orderMap_.erase(askOrder->id);

                orderPool_.deallocate(askOrder);

                if(bestAskLevel->isEmpty()){
                    delete bestAskLevel;
                    asks_.erase(asks_.begin());
                }
            }
        }
    }

    void OrderBook::cancelOrder(OrderId id){
        auto it = orderMap_.find(id);
        if(it == orderMap_.end()){
            std::cout << "[Error] Cancel failed: Order #" << id << " not found.\n";
            return;
        }

        Order* order = it->second;
        LimitLevel* level = getLimitLevel(order->price, order->side);

        // 1. Remove from the Linked List (O(1))
        level->remove(order);

        // 2. Remove from the Index (O(1))
        orderMap_.erase(it);

        // 3. Return memory to pool (O(1))
        orderPool_.deallocate(order);

        // 4. Cleanup empty levels to keep the tree small
        if(level->isEmpty()){
            if(order->side == Side::Buy){
                bids_.erase(order->price);
            }
            else{
                asks_.erase(order->price);
            }
            delete level;
        }
        std::cout << ">>> Cancelled Order #" << id << "\n";
    }
}