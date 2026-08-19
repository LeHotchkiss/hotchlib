#ifndef HOTCHLIB_API_STACK_H
#define HOTCHLIB_API_STACK_H

#include "hlib/Array.h"

namespace hlib {
    template <typename value_t, typename alloc_t, alloc_t* t_pAlloc> 
    class CStack : private CArray<value_t, alloc_t, t_pAlloc> {
        using this_t = CStack<value_t, alloc_t, t_pAlloc>;
        using parent_t = CArray<value_t, alloc_t, t_pAlloc>;

        public:
            CStack() = default;

            value_t& Head() {
                return this->Last();
            }

            const value_t& Head() const {
                return this->Last();
            }

            void Pop() {
                this->PopBack();
            }

            void Push(const value_t& Value) {
                this->PushBack(Value);
            }

            void Push(value_t&& Value) {
                this->PushBack(std::move(Value));
            }

            value_t& operator[](size_t iIndex) {
                return this->m_pMemory[iIndex];
            }

            size_t Length() const {
                return parent_t::Length();
            }

            void Strink() {
                parent_t::Shrink();
            }

            void Reserve(size_t iSize) {
                parent_t::Reserve(iSize);
            } 

            void Clear() {
                parent_t::Clear();
            }
    };

    template <typename value_t> using default_stack_t = CStack<value_t, CDefaultAllocator, &g_defaultAlloc>;
}

#endif
