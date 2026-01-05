#pragma once
#include <map>
#include <unordered_map>
#include "LimitLevel.h"

namespace LOB {

    class OrderBook {
        private:
            std::map<Price, LimitLevel*, std::greater<Price>> bids_;
            std::map<Price, LimitLevel*, std::less<Price>> asks_;
            std::unordered_map<OrderId, Order*> orderMap_;

        public:
            OrderBook() = default;
            ~OrderBook();

            void addOrder(OrderId id, Price price, Quantity qty, Side side);

            void printBook() const;
        
        private:
            LimitLevel* getLimitLevel(Price price, Side side);
    };
}