/**
 * @file LockFreeQueue.h
 * @brief Defines a lock-free Single-Producer Single-Consumer (SPSC) ring buffer.
 * @details This class implements a circular buffer that allows two threads (Network and Engine)
 * to exchange data without using mutexes. It relies on C++20 std::atomic with Acquire-Release memory
 * ordering to ensure data consistency with minimal CPU overhead.
 */
#pragma once
#include <vector>
#include <atomic>

namespace LOB {

    /**
     * @class LockFreeQueue
     * @brief A fixed-size, thread-safe queue for SPSC communication.
     * @tparam T the type of data to be stored (e.g., OrderRequest).
     */
    template <typename T>
    class LockFreeQueue {
        private:
            std::vector<T> buffer_;

            // alignas(64) prevents "False Sharing" by ensuring head and tail
            // reside on different cache lines.
            std::atomic<size_t> head_ = {0}; /**<Read Index (Owned by Consumer) */
            std::atomic<size_t> tail_ = {0}; /**<Write Index (Owned by Producer) */
            size_t capacity_;

        public:
            /**
             * @brief Construct a new Lock Free Queue.
             * @param size The maximum capacity of the queue.
             */
            explicit LockFreeQueue(size_t size) : buffer_(size + 1), capacity_(size + 1) {}

            /**
             * @brief Pushes an item onto the queue (Producer only).
             * @param item The data to the push.
             * @return true if the item was successully added, false if the queue is full.
             * @note Uses std::memory_order_release to publish the new tail index.
             */
            bool push(const T& item){
                // Relaxed: We are the only thread writing to tail_, so we just need the value.
                size_t currentTail = tail_.load(std::memory_order_relaxed);
                size_t nextTail = (currentTail + 1) % capacity_;

                // Acquire: Sync with Consumer. Ensure we read the latest 'head_'
                // to avoid overwriting unread data.
                if(nextTail == head_.load(std::memory_order_acquire)) {
                    return false;
                }

                buffer_[currentTail] = item;

                // Release: Publish the new tail. Guarantees that the write to 'buffer_'
                // is visible to the consumer BEFORE they see the new tail index.
                tail_.store(nextTail, std::memory_order_release);
                return true;
            }

            /**
             * @brief Pops an item from the queue (Consumer only).
             * @param item Reference where the popped data will be stored.
             * @return true if an item was retrieved otherwise, false if the queue was empty.
             * @note Uses std::memory_order_acquire to synch with the producer.
             */
            bool pop(T& item){
                // Released: We are the only thread writing to head_.
                size_t currentHead = head_.load(std::memory_order_relaxed);

                // Acquire: Sync with Producer. Ensure we read the latest 'tail_'
                // to see if the new data is available.
                if(currentHead == tail_.load(std::memory_order_acquire)){
                    return false;
                }

                item = buffer_[currentHead];

                // Release: Notify Producer that a slot has been freed.
                head_.store((currentHead + 1) % capacity_, std::memory_order_release);
                return true;
            }
    };
}