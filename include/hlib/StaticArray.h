#ifndef HOTCHLIB_API_STATIC_ARRAY_H
#define HOTCHLIB_API_STATIC_ARRAY_H

#include <stddef.h>

#include <utility>

namespace hlib {
    template <typename value_t, size_t t_iLength>
    class CStaticArray {
        static_assert(t_iLength > 0, "Static array length must be greater then zero");

        value_t m_aData[t_iLength];

        using this_t = CStaticArray<value_t, t_iLength>;

        public:
            constexpr CStaticArray() = default;

            template <typename... args_t>
            constexpr CStaticArray(args_t&&... args) : m_aData{ std::forward<args_t>(args)... } {
                static_assert(sizeof...(args) == t_iLength, "Wrong number of static array arguments");
            }
            
            constexpr CStaticArray(const value_t& filler) {
                for(size_t i = 0; i < t_iLength; i++) {
                    m_aData[i] = filler;
                }
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
