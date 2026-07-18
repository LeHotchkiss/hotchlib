#ifndef HOTCHLIB_API_BINARY_MAP_H
#define HOTCHLIB_API_BINARY_MAP_H

#include "hlib/Array.h"
#include "hlib/Pair.h"

namespace hlib {
    /*
        Key-Value table based on the almighty binary search

        Read / Search - O(log2N)
        Write / Erase - O(N)

        key_t must have defined '==' and '<' operators
    */
    
    template <typename key_t, typename value_t, typename alloc_t = CDefaultAllocator<CPair<key_t, value_t>>>
    class CBinTable {
        CArray<CPair<key_t, value_t>, alloc_t> m_aData;
        
        int64_t BinarySearch(const key_t& Key) const {
            if (m_aData.Length() == 0) {
                return -1;  // -(0 + 1) = -1
            }
            
            int64_t iL = 0;
            int64_t iR = (int64_t)(m_aData.Length()) - 1;
            
            while (iL <= iR) {
                int64_t iM = iL + (iR - iL) / 2;
                const key_t& midKey = m_aData[iM].Key;
                
                if (midKey == Key) {
                    return iM;
                } else if (midKey < Key) {
                    iL = iM + 1;
                } else {
                    iR = iM - 1;
                }
            }
        
            return -iL - 1;
        }
        
        public:
            typedef value_t value_t;
            typedef key_t key_t;
            typedef CPair<key_t, value_t> pair_t;
            typedef CArray<pair_t, alloc_t> pairs_t;

            CBinTable() = default;

            const pairs_t& Data() {
                return m_aData;
            }
            
            // Insert a pair
            void Insert(const key_t& Key, const value_t& Value) {
                if (m_aData.Length() == 0) {
                    CPair Pair(std::move(Key), std::move(Value));
                    m_aData.PushBack(std::move(Pair));
                    return;
                }
                
                int64_t iPos = BinarySearch(Key);
                
                if (iPos >= 0) {
                    m_aData[(size_t)iPos].Value = Value;
                } else {
                    size_t iInsertPos = (size_t)(-iPos - 1);

                    CPair Pair(std::move(Key), std::move(Value));

                    m_aData.InsertAt(iInsertPos, std::move(Pair));
                }
            }
            
            /*
                Find the key`s index in the internal array
                If there is no such a key, (size_t)(-1) is returned
            */
            size_t FindIndex(const key_t& Key) const {
                int64_t iPos = BinarySearch(Key);

                if(iPos < 0) { return (size_t)(-1); }

                return (size_t)iPos;
            }
            
            /*
                Direct index access with O(1) complexity
            */
            value_t& Index(size_t iIndex) {
                return m_aData[iIndex].Value;
            }

            /*
                Direct index access with O(1) complexity
            */
            key_t& IndexKey(size_t iIndex) {
                return m_aData[iIndex].Key;
            }

            /*
                Direct index access with O(1) complexity
            */
            const value_t& Index(size_t iIndex) const {
                return m_aData[iIndex].Value;
            }
            
            // Insert a pair
            void Insert(key_t&& Key, value_t&& Value) {
                if (m_aData.Length() == 0) {
                    CPair Pair(Key, Value);
                    m_aData.PushBack(std::move(Pair));
                    return;
                }
                
                int64_t iPos = BinarySearch(Key);
                
                if (iPos >= 0) {
                    m_aData[(size_t)iPos].Value = std::move(Value);
                } else {
                    size_t iInsertPos = (size_t)(-iPos - 1);

                    CPair Pair(Key, Value);

                    m_aData.InsertAt(iInsertPos, std::move(Pair));
                }
            }

            bool HasKey(const key_t& Key) const {
                return BinarySearch(Key) >= 0;
            }
            
            /*
                Attempt getting a value by this key.
                If there is no such a pair, NULL is returned
            */
            value_t* At(const key_t& Key) {
                int64_t iPos = BinarySearch(Key);
                if (iPos >= 0) {
                    return &m_aData[(size_t)iPos].Value;
                }
                return NULL;
            }
            
            /*
                Attempt getting a value by this key.
                If there is no such a pair, NULL is returned
            */
            const value_t* At(const key_t& Key) const {
                int64_t iPos = BinarySearch(Key);
                if (iPos >= 0) {
                    return &m_aData[(size_t)iPos].Value;
                }
                return NULL;
            }
            
            /*
                Get a value by key, or insert a new pair
                if there is none
            */
            value_t& operator[](const key_t& Key) {
                int64_t iPos = BinarySearch(Key);
                
                if (iPos >= 0) {
                    return m_aData[(size_t)iPos].Value;
                } else {
                    size_t iInsertPos = (size_t)(-iPos - 1);

                    value_t DefaultValue;
                    CPair Pair(std::move(Key), std::move(DefaultValue));

                    m_aData.InsertAt(iInsertPos, std::move(Pair));
                    return m_aData[iInsertPos].Value;
                }
            }
            
            bool Erase(const key_t& Key) {
                int64_t iPos = BinarySearch(Key);
                if (iPos >= 0) {
                    m_aData.RemoveAt((size_t)iPos);
                    return true;
                }
                return false;
            }
            
            void Clear() {
                m_aData.Clear();
            }

            void Shrink() {
                m_aData.Shrink();
            }
            
            size_t Length() const {
                return m_aData.Length();
            }

            void Reserve(size_t iCapacity) {
                m_aData.Reserve(iCapacity);
            }
    };
}

#endif
