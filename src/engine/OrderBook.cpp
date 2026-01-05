#include "LOB/OrderBook.h"
#include <iostream>

namespace LOB {

    OrderBook::~OrderBook() {
        for(auto& pair : orderMap_){
            delete pair.second;
        }

        for(auto& pair : bids_) delete pair.second;
        for(auto& pair : asks_) delete pair.second;
    }

    void OrderBook::addOrder(OrderId id, Price price, Quantity qty, Side side){
        if(orderMap_.find(id) != orderMap_.end()){
            return;
        }

        Order* order = new Order(id, price, qty, side);
        orderMap_[id] = order;

        LimitLevel* level = getLimitLevel(price, side);

        level->append(order);
    }

    void OrderBook::printBook() const {
        std::cout << "\n--- ORDER BOOK SNAPSHOT ---\n";

        std::cout << "ASKS (Sellers):\n";
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
}