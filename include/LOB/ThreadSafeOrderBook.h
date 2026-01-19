#pragma once
#include "OrderBook.h"
#include "SpinLock.h"
#include <thread>

namespace LOB{
    class ThreadSafeOrderBook{
        private:
            OrderBook book_;
            SpinLock lock_;
        public:
            void addOrder(OrderId id, Price price, Quantity qty, Side side){
                lock_.lock();
                book_.addOrder(id, price, qty, side);
                lock_.unlock();
            }

            void cancelOrder(OrderId id){
                lock_.lock();
                book_.cancelOrder(id);
                lock_.unlock();
            }

            void printBook() const {
                const_cast<SpinLock&>(lock_).lock();
                book_.printBook();
                const_cast<SpinLock&>(lock_).unlock();
            }
    };
}