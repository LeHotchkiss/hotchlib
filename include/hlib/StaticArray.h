#ifndef HOTCHLIB_API_STATIC_ARRAY_H
#define HOTCHLIB_API_STATIC_ARRAY_H

#include <stddef.h>
#include <string.h>

namespace hlib {
    template <typename value_t, size_t t_iLength>
    class CStaticArray {
        value_t m_aData[t_iLength];

        using this_t = CStaticArray<value_t, t_iLength>;

        public:
            constexpr CStaticArray() = default;

            constexpr CStaticArray(const value_t& filler) {
                for(size_t i = 0; i < t_iLength; i++) {
                    m_aData[i] = filler;
                }
            }

            constexpr CStaticArray(const this_t& other) {
                for(size_t i = 0; i < t_iLength; i++) {
                    m_aData[i] = other[i];
                }
            }

            constexpr CStaticArray(this_t&& other) {
                memcpy(m_aData, other.m_aData, sizeof(m_aData));
                memset(other.m_aData, 0, sizeof(other.m_aData));
            }

            constexpr this_t& operator=(const this_t& other) {
                for(size_t i = 0; i < t_iLength; i++) {
                    m_aData[i] = other[i];
                }
            }

            constexpr this_t& operator=(this_t&& other) {
                memcpy(m_aData, other.m_aData, sizeof(m_aData));
                memset(other.m_aData, 0, sizeof(other.m_aData));
            }
            
            constexpr value_t& operator[](size_t index) {
                return m_aData[index];
            }

            constexpr const value_t& operator[](size_t index) const {
                return m_aData[index];
            }

            constexpr value_t* At(size_t index) {
                return (index < t_iLength) ? (&m_aData[index]) : NULL;
            }

            constexpr const value_t* At(size_t index) const {
                return (index < t_iLength) ? (const value_t*)(&m_aData[index]) : NULL;
            }

            constexpr size_t Length() const { return t_iLength; }

            constexpr value_t* Data() { return (value_t*)(m_aData); }
            constexpr const value_t* Data() const { return (const value_t*)(m_aData); }
    };

    template <typename value_t, size_t t_iSize>
    using static_array_t = CStaticArray<value_t, t_iSize>;
}

#endif
