/**
 * @file SpinLock.h
 * @brief Defines a low-latency user-space lock using atomic flags.
 * @details Standard mutexes (std::mutex) put threads to sleep (syscall) when contention occurs, causing a context
 * switch overhead of ~10 microseconds.
 * * This SpinLock uses a "Busy Wait" strategy: the thread loops on the CPU checking the falg. This burns 100% CPU core but reacts in nanoseconds (10-50ns).
 * Ideal for critical sections that are extremely short (like updating a pointer).
 */
#pragma once
#include <atomic>

namespace LOB{
    /**
     * @class SpinLock
     * @brief A lock implementation that busy-waits instead of sleeping.
     */
    class SpinLock{
        private:
            // ATOMIC_FLAG_INIT is only state guaranteed to be lock-free on all architectures.
            std::atomic_flag flag = ATOMIC_FLAG_INIT;

        public:
            /**
             * @brief Acquires the lock.
             * @details Loops continously until the flag is cleared.
             * Uses std::memory_order_acquire to ensure no subsequent reads/writes are reordered before this point.
             */
            void lock(){
                // test_and_set(): Returns true if already set, false if we successfully set it.
                // We spin while it returns true (meaning someone else has it).
                while(flag.test_and_set(std::memory_order_acquire)){
                    // In a production engine, we might add, but here we have skipped->
                    // _mm_pause(); 
                }
            }

            /**
             * @brief Releases the lock.
             * @details Clears the flag. Uses std::memory_order_release to ensure all previous writes are visible to other threads before the lock is released.
             */
            void unlock(){
                flag.clear(std::memory_order_release);
            }
    };
}