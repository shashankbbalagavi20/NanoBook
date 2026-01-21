/**
 * @file LimitLevel.cpp
 * @brief Implementation of the LimitLevel class.
 * @details Manages the Doubly Linked List logic for adding and removing orders at a specific price level.
 */
#include "LOB/LimitLevel.h"

namespace LOB{

    LimitLevel::LimitLevel(Price p): price_(p), totalVolume_(0), head_(nullptr), tail_(nullptr) {}

    void LimitLevel::append(Order* order){
        if(!head_){
            // Case 1: Empty List
            // The new order is now both the Head and the Tail.
            head_ = order;
            tail_ = order;
            order->next = nullptr;
            order->prev = nullptr;
        }
        else{
            // Case 2: Non-Empty List
            // Link the current tail to the new order
            tail_->next = order;
            order->prev = tail_;
            order->next = nullptr;

            // Update the tail pointer to the new order
            tail_ = order;
        }

        // Cache the volume update (O(1))
        totalVolume_ += order->quantity;
    }

    void LimitLevel::remove(Order* order){
        // Step 1: Unlink from the 'previous' node
        if(order->prev){
            // We are in the middle or tail
            order->prev->next = order->next;
        }
        else{
            // We are the Head (order->prev is null)
            // The new head becomes the next node
            head_ = order->next;
        }

        // Step 2: Unlink from the 'next' node
        if(order->next){
            // We are in the middle or head
            order->next->prev = order->prev;
        }
        else{
            // We are the Tail (order->next is null)
            // The new tail becomes the previous node
            tail_ = order->prev;
        }

        // Step 3: Clean up the detached order
        order->next = nullptr;
        order->prev = nullptr;

        // Update cached volume
        totalVolume_ -= order->quantity;
    }

    bool LimitLevel::isEmpty() const {
        return head_ == nullptr;
    }
}