#pragma once
#include <map>
#include <unordered_map>
#include "LimitLevel.h"
#include "ObjectPool.h"

namespace LOB {

    class OrderBook {
        private:
            std::map<Price, LimitLevel*, std::greater<Price>> bids_;
            std::map<Price, LimitLevel*, std::less<Price>> asks_;
            std::unordered_map<OrderId, Order*> orderMap_;

            ObjectPool<Order> orderPool_;

        public:
            OrderBook();
            ~OrderBook();

            void addOrder(OrderId id, Price price, Quantity qty, Side side);

            void printBook() const;
        
        private:
            LimitLevel* getLimitLevel(Price price, Side side);
            void match();
    };
}