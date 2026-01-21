/**
 * @file ThreadSafeOrderBook.h
 * @brief A thread-safe wrapper (Decorator) for the standard OrderBook.
 * @details
 * This class applies the "Monitor" synchronization pattern:
 * 1. It holds an instance of OrderBook.
 * 2. It owns a SpinLock.
 * 3. It exposes the same interface as OrderBook, but wraps every method call in a lock()/unlock() block.
 * This ensures that multiple threads (e.g., Worker Threads) can safely submit orders simultaneously without causing race conditions or memory corruption.
 */
#pragma once
#include "OrderBook.h"
#include "SpinLock.h"
#include <thread>

namespace LOB{
    class ThreadSafeOrderBook{
        private:
            // The underlying engine (not thread-safe by itself)
            OrderBook book_;

            // The guard that protects the engine
            // 'mutable' keyword would be cleaner, but const_cast works too.
            SpinLock lock_;

        public:
            /**
             * @brief Thread-safe wrapper for adding an order.
             * @details
             * 1. Acquires SpinLock (Busy Wait).
             * 2. Delegates to OrderBook::addOrder().
             * 3. Release SpinLock.
             */
            void addOrder(OrderId id, Price price, Quantity qty, Side side){
                lock_.lock();
                // --- CRITICAL SECTION START ---
                book_.addOrder(id, price, qty, side);
                // --- CRITICAL SECTION END ---
                lock_.unlock();
            }

            /**
             * @brief Thread-safe wrapper for cancelling an order.
             */
            void cancelOrder(OrderId id){
                lock_.lock();
                book_.cancelOrder(id);
                lock_.unlock();
            }

            /**
             * @brief Thread-safe wrapper for printing the book.
             * @note Uses const_cast because lock() modifies the internal state of the SpinLock (setting the flag),
             * even though the OrderBook itself is not modified.
             */
            void printBook() const {
                // We must cast away constness to acquire the lock
                const_cast<SpinLock&>(lock_).lock();
                book_.printBook();
                const_cast<SpinLock&>(lock_).unlock();
            }
    };
}