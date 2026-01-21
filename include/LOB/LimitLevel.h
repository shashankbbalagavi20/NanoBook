/**
 * @file LimitLevel.h
 * @brief Defines the LimitLevel class, representing a price level in the Order Book.
 * @details A LimitLevel functions as a FIFO queue (Doubly Linked List) for all orders at a specific price.
 * It enforces Price-Time Priority by appending new orders to the tail and matching from the head.
 */
#pragma once
#include "Order.h"

namespace LOB {
    /**
     * @class LimitLevel
     * @brief A container for orders at a single price level.
     * @details This class manages a doubly linked list of Order objects.
     * Design Choices:
     * - **Intrusive List:** Uses the 'next' and 'prev' pointers inside the Order struct to avoid dynamic allocation of list nodes.
     * - **O(1) Operations:** Appending, removing, and checking volume are all constant time.
     */  
    class LimitLevel {
        private:
            Price price_;
            Quantity totalVolume_;

            // Head of the queue (First order to be executed - Time Priority)
            Order* head_ = nullptr;

            // Tail of the queue (Last order added)
            Order* tail_ = nullptr;

        public:
            /**
             * @brief Construct a new Limit Level object.
             * @param p The price for this Level.
             */
            explicit LimitLevel(Price p);

            /**
             * @brief Adds an order to the end of the level (Tail).
             * @param order Pointer to the order to add.
             * @note Complexity: O(1)
             */
            void append(Order* order);

            /**
             * @brief Removes a specific order from the level.
             * @details Handles unlinking from head/tail/middle of the list.
             * @param order Pointer to the order to remove.
             * @note Complexity: O(1)
             */
            void remove(Order* order);

            /**
             * @brief Checks if the level has no orders.
             * @return true if empty, false otherwise.
             */
            bool isEmpty() const;

            /**
             * @brief Gets the price of this level.
             */
            Price getPrice() const { return price_;}

            /**
             * @brief Gets the total liquidity available at this price.
             * @return Total quantity of all orders in this level.
             */
            Quantity getVolume() const { return totalVolume_;}

            /**
             * @brief Access the oldest order at this level (for matching).
             * @return Pointer to the head order, or nullptr if empty.
             */
            Order* getHead() const { return head_; }
    };
}