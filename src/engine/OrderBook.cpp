#include "LOB/OrderBook.h"
#include <iostream>
#include <algorithm>

namespace LOB {

    OrderBook::OrderBook() : orderPool_(10000) {}

    OrderBook::~OrderBook() {
        for(auto& pair : bids_) delete pair.second;
        for(auto& pair : asks_) delete pair.second;
    }

    void OrderBook::addOrder(OrderId id, Price price, Quantity qty, Side side){
        if(orderMap_.find(id) != orderMap_.end()){
            return;
        }

        Order* order = orderPool_.allocate(id, price, qty, side);
        if(!order) return;

        orderMap_[id] = order;

        LimitLevel* level = getLimitLevel(price, side);

        level->append(order);

        match();
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

    void OrderBook::match(){
        while (true){
            if(bids_.empty() || asks_.empty()){
                break;
            }

            auto bestBidLevel = bids_.begin()->second;
            auto bestAskLevel = asks_.begin()->second;

            if(bestBidLevel->getPrice() < bestAskLevel->getPrice()){
                break;
            }

            Order* bidOrder = bestBidLevel->getHead();
            Order* askOrder = bestAskLevel->getHead();

            Quantity quantity = std::min(bidOrder->quantity, askOrder->quantity);

            std::cout << ">>> TRADE EXECUTE: " << quantity << " shares @ " << bestAskLevel->getPrice() << " (Bid #" << bidOrder->id << " vs Ask #" << askOrder->id << ")\n";

            bidOrder->quantity -= quantity;
            askOrder->quantity -= quantity;

            if(bidOrder->quantity == 0){
                bestBidLevel->remove(bidOrder);
                orderMap_.erase(bidOrder->id);

                orderPool_.deallocate(bidOrder);

                if(bestBidLevel->isEmpty()){
                    delete bestBidLevel;
                    bids_.erase(bids_.begin());
                }   
            }

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
        level->remove(order);
        orderMap_.erase(it);
        orderPool_.deallocate(order);
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