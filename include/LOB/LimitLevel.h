#pragma once
#include "Order.h"

namespace LOB {
    class LimitLevel {
        private:
            Price price_;
            Quantity totalVolume_;

            Order* head_ = nullptr;
            Order* tail_ = nullptr;

        public:
            explicit LimitLevel(Price p);

            void append(Order* order);
            void remove(Order* order);

            bool isEmpty() const;
            Price getPrice() const { return price_;}
            Quantity getVolume() const { return totalVolume_;}

            Order* getHead() const { return head_; }
    };
}