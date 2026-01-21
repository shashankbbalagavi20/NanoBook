/**
 * @file OrderBook.h
 * @brief Defines the core matching engine logic.
 * @details The OrderBook maintains the state of all active Buy and Sell orders.
 * It uses a hybrid data structure approach:
 * 1. **Price Tress (std::map):** Keep orders sorted by Price.
 * - Bids: Sorted Descending (Highest price first).
 * - Asks: Sorted Ascending (Lowest price first).
 * 2. **Order Map (std::unordered_map):** Maps OrderId -> Order* for O(1) lookups/cancellations.
 * 3. **Memory Pool:** All order objects are allocated from a pre-allocated slab to avoid heap fragmentation.
 */
#pragma once
#include <map>
#include <unordered_map>
#include "LimitLevel.h"
#include "ObjectPool.h"

namespace LOB {

    class OrderBook {
        private:
            // Bids: Buyers want to pay LESS, but priority goes to those paying MORE.
            // Comparator: std::greater (Descending: 101, 100, 99...)
            std::map<Price, LimitLevel*, std::greater<Price>> bids_;

            // Asks: Sellers want to receive MORE, but priority goes to those asking LESS.
            // Comparator: std::less (Ascending: 100, 101, 102...)
            std::map<Price, LimitLevel*, std::less<Price>> asks_;

            // Fast Lookup Table: Enables O(1) cancellation by Order ID.
            std::unordered_map<OrderId, Order*> orderMap_;

            // Memory Manager: Pre-allocated pool for 100,000 orders.
            ObjectPool<Order> orderPool_;

        public:
            /**
             * @brief Construct a new Order Book.
             * @note Initializes the ObjectPool with a fixed capacity.
             */
            OrderBook();

            /**
             * @brief Destroy the Order Book and clean up limit levels.
             */
            ~OrderBook();
            
            /**
             * @brief Submits a new order to the book.
             * @details
             * 1. Attempts to match immediately (crosssing the spread).
             * 2. If remaining quantity > 0, posts to the book at the specific LimitLevel.
             * @param id Unique Order ID.
             * @param price Limit Price.
             * @param qty Total Quantity.
             * @param side Buy or Sell.
             */
            void addOrder(OrderId id, Price price, Quantity qty, Side side);

            /**
             * @brief Cancels an existing order.
             * @details 
             * 1. Looks up Order* in orderMap_ (O(1)).
             * 2. Unlinks from LimitLevel (O(1)).
             * 3. Returns Order to ObjectPool (O(1)).
             * @param id The ID of the order to cancel.
             */
            void cancelOrder(OrderId id);
            
            /**
             * @brief Prints the top levels of the book to the console (Visualization).
             */
            void printBook() const;
        
        private:
            /**
             * @brief Helper to find or create a LimitLevel for a specific price.
             * @return Pointer to the LimitLevel.
             */
            LimitLevel* getLimitLevel(Price price, Side side);

            /**
             * @brief The Core Matching Algorithm.
             * @details Checks if Best Bid >= Best Ask. If so, executes trades until price no longer cross or liquidity is exhausted.
             */
            void match();
    };
}