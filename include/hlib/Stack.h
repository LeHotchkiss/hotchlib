#ifndef HOTCHLIB_API_STACK_H
#define HOTCHLIB_API_STACK_H

#include "hlib/Array.h"

namespace hlib {
    template <typename value_t, typename alloc_t = CDefaultAllocator<value_t>> class CStack {
        CArray<value_t, alloc_t> m_aData;

        public:
            CStack() = default;

            value_t& Head() {
                return m_aData.Last();
            }

            const value_t& Head() const {
                return m_aData.Last();
            }

            void Pop() {
                m_aData.PopBack();
            }

            void Push(const value_t& Value) {
                m_aData.PushBack(Value);
            }

            void Push(value_t&& Value) {
                m_aData.PushBack(std::move(Value));
            }

            value_t& operator[](size_t iIndex) {
                return m_aData[iIndex];
            }

            size_t Length() const {
                return m_aData.Length();
            }

            void Strink() {
                m_aData.Shrink();
            }

            void Reserve(size_t iSize) {
                m_aData.Reserve(iSize);
            } 

            void Clear() {
                m_aData.Clear();
            }
    };
}

#endif
