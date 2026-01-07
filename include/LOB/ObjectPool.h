#pragma once
#include <vector>
#include <numeric>

namespace LOB{
    template <typename T>
    class ObjectPool{
        private:
            std::vector<T> pool_;
            std::vector<size_t> freeIndices_;

        public:
            explicit ObjectPool(size_t size){
                pool_.resize(size);
                freeIndices_.reserve(size);
                for(size_t i=0; i<size; ++i){
                    freeIndices_.push_back(i);
                }
            }

            template <typename... Args>
            T* allocate(Args&&... args){
                if(freeIndices_.empty()){
                    return nullptr;
                }

                size_t idx = freeIndices_.back();
                freeIndices_.pop_back();

                T* obj = &pool_[idx];

                *obj = T(std::forward<Args>(args)...);

                return obj;
            }

            void deallocate(T* obj){
                size_t idx = obj - &pool_[0];
                freeIndices_.push_back(idx);
            }
    };
}