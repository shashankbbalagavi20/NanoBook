/**
 * @file Order.h
 * @brief Defines the order structure, serving as the primary node in the Order Book.
 * @details The Order struct is designed for high-performance memory layout:
 * - **Instrusive Linked List:** Contains 'next' and 'prev' pointer directly, avoiding seperate allocations for list nodes.
 * - **Memory Packing:** Uses uint8_t for side to minimize structure padding.
 */
#pragma once
#include <cstdint>

namespace LOB {

    // Strong typing prevents accidental swapping of Price and ID
    using OrderId = uint64_t;
    using Price = uint64_t;
    using Quantity = uint64_t;

    /**
     * @enum Side
     * @brief Represents the side of the book (Bid/Ask).
     * @note Underlying type is uint8_t to save memory (1 byte vs 4 bytes).
     */
    enum class Side : uint8_t { Buy, Sell };

    /**
     * @struct Order
     * @brief Represents a single limit order.
     * @details This struct acts as both the business data holder and the linked list node.
     * It is desinged to be allocated via the ObjectPool to ensure cache locality.
     */
    struct Order {
        // --- Business Data ---
        OrderId id;
        Price price;
        Quantity quantity;
        Side side;

        // --- Intrusive Pointers ---
        // These allow the Order to live in a Doubly Linked List (LimitLevel) without requiring an 
        // external "Node" wrapper class. This reduces cache misses by keeping metadata close to the data.
        Order* next = nullptr;
        Order* prev = nullptr;

        Order() = default;

        /**
         * @brief Construct a new order object
         * @note Typically called by ObjectPool::allocate()
         */
        Order(OrderId id, Price price, Quantity qty, Side side) : id(id), price(price), quantity(qty), side(side) {}
    };
}