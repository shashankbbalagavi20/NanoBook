#include "LOB/LimitLevel.h"

namespace LOB{

    LimitLevel::LimitLevel(Price p): price_(p), totalVolume_(0), head_(nullptr), tail_(nullptr) {}

    void LimitLevel::append(Order* order){
        if(!head_){
            // Case: Empty List
            head_ = order;
            tail_ = order;
            order->next = nullptr;
            order->prev = nullptr;
        }
        else{
            // Case: Add to tail
            tail_->next = order;
            order->prev = tail_;
            order->next = nullptr;
            tail_ = order;
        }
        totalVolume_ += order->quantity;
    }

    void LimitLevel::remove(Order* order){
        // Unlink from previous
        if(order->prev){
            order->prev->next = order->next;
        }
        else{
            head_ = order->next;
        }

        // Unlink from next
        if(order->next){
            order->next->prev = order->prev;
        }
        else{
            tail_ = order->prev;
        }

        order->next = nullptr;
        order->prev = nullptr;
        totalVolume_ -= order->quantity;
    }

    bool LimitLevel::isEmpty() const {
        return head_ == nullptr;
    }
}