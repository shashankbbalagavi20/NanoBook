/**
 * @file ObjectPool.h
 * @brief Defines a pre-allocated memory pool (Slab Allocator) for fast object management.
 * @details Calling 'new' or 'malloc' is avoided because it triggers a system call (context switch)
 * and non-deterministic heap fragmentation.
 * This ObjectPool pre-allocates a contiguos block of memory (std::vector)
 * - Allocation: O(1) - Pops a free index from the stack.
 * - Deallocation: O(1) - Pushes the index back onto the stack.
 * - Cache Friendliness: LIFO (Last-In, First-Out) strategy ensures we reuse the most recently released object(hot cache).
 */
#pragma once
#include <vector>
#include <numeric>

namespace LOB{
    /**
     * @class ObjectPool
     * @brief Template class for managing a fixed-size pool of objects.
     * @tparam T The type of object to store (e.g., Order).
     */
    template <typename T>
    class ObjectPool{
        private:
            // Contiguos memory block. Friendly to CPU prefetcher.
            std::vector<T> pool_;

            // Stack of available indices.
            // We use indices instead of pointers to ensure stability if vector resizes.
            std::vector<size_t> freeIndices_;

        public:
            /**
             * @brief Construct a new object pool.
             * @param size The total capacity of the pool.
             * @note This triggers strict pre-allocation. All objects are default-constructed immediately.
             */
            explicit ObjectPool(size_t size){
                pool_.resize(size);
                freeIndices_.reserve(size);

                // Initialize the free list with all indices [0, size-1]
                // It will fill in reverse order or standard order; standard is fine here.
                for(size_t i=0; i<size; ++i){
                    freeIndices_.push_back(i);
                }
            }

            /**
             * @brief Allocates an object from the pool.
             * @tparam Args Variadic template arguments for T's constructor.
             * @param args Arguments to forward to the object constructor.
             * @return T* Pointer to the initialized object, or nullptr if pool is full.
             * @note Uses Perfect Forwarding to construct the object in-place (via assignment).
             * @note Complexity: O(1)
             */
            template <typename... Args>
            T* allocate(Args&&... args){
                if(freeIndices_.empty()){
                    return nullptr; // Pool Exhausted
                }

                // LIFO Strategy: Take the last freed index (Hot Cache)
                size_t idx = freeIndices_.back();
                freeIndices_.pop_back();

                T* obj = &pool_[idx];

                // Re-initialize the object using assignment.
                // Note: For complex objects, placement new might be preferred,
                // but for POD (Plain Old Data) structs like Order, this is extremely fast.
                *obj = T(std::forward<Args>(args)...);

                return obj;
            }

            /**
             * @brief Returns an object to the pool.
             * @param obj Pointer to the object to deallocate.
             * @note Complexity: O(1) - Uses pointer arithmetic to calculate index.
             */
            void deallocate(T* obj){
                // Pointer Arithmetic: Calculate index based on memory address offset.
                // This avoids searching the vector for the object.
                // Formula: (Address of Obj - Address of Start)
                size_t idx = obj - &pool_[0];

                // Mark index as free
                freeIndices_.push_back(idx);
            }
    };
}