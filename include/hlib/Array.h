#ifndef HOTCHLIB_API_LIST_H
#define HOTCHLIB_API_LIST_H

#include <new>
#include <initializer_list>

#include "hlib/Allocator.h"

namespace hlib {
    /*
        Growable array
    */
    template <typename value_t, typename alloc_t = CDefaultAllocator<value_t>> 
    class CArray {
        public:
            CArray() = default;
            
            CArray(const CArray<value_t>& aOther) : m_iAllocated(aOther.m_iAllocated), m_iSize(aOther.m_iSize) {
                if (m_iAllocated > 0) {
                    m_pMemory = alloc_t::Malloc(m_iAllocated);
                    for (size_t i = 0; i < m_iSize; ++i) {
                        new (&m_pMemory[i]) value_t(aOther.m_pMemory[i]);
                    }
                }
            }
            
            CArray(CArray<value_t>&& Other)  
                : m_pMemory(Other.m_pMemory), m_iAllocated(Other.m_iAllocated), m_iSize(Other.m_iSize) {
                Other.m_pMemory = NULL;
                Other.m_iAllocated = 0;
                Other.m_iSize = 0;
            }

            CArray(std::initializer_list<value_t> List) : m_iSize(List.size()), m_iAllocated(List.Size()) {
                m_pMemory = alloc_t::Malloc(m_iSize);

                int i = 0;
                for (auto I = List.begin(); I != List.end(); ++I) {
                    new (&m_pMemory[i]) value_t(*I);
                    i++;
                }
            }
            
            CArray(const value_t* pSource, size_t iSrcLen) : m_iSize(iSrcLen), m_iAllocated(iSrcLen) {
                if (iSrcLen > 0) {
                    m_pMemory = alloc_t::Malloc(iSrcLen);
                    for (size_t i = 0; i < iSrcLen; ++i) {
                        new (&m_pMemory[i]) value_t(pSource[i]);
                    }
                }
            }
            
            CArray(size_t iInitialCapacity) : m_iSize(0), m_iAllocated(iInitialCapacity) {
                if (iInitialCapacity != 0) {
                    m_pMemory = alloc_t::Malloc(iInitialCapacity);
                }
            }
            
            ~CArray() {
                Clear();
                if (m_pMemory != NULL) {
                    alloc_t::Free(m_pMemory);
                }
            }
            
            CArray& operator=(const CArray<value_t>& aOther) {
                Clear();
                
                m_pMemory = alloc_t::Realloc(m_pMemory, aOther.m_iAllocated);

                m_iAllocated = aOther.m_iAllocated;
                m_iSize = aOther.m_iSize;

                for (size_t i = 0; i < m_iSize; ++i) {
                    m_pMemory[i] = aOther.m_pMemory[i];
                }
                
                return *this;
            }
            
            CArray& operator=(CArray<value_t>&& Other) {
                if (this != &Other) {
                    Clear();
                    if (m_pMemory != NULL) {
                        alloc_t::Free(m_pMemory);
                    }
                    
                    m_pMemory = Other.m_pMemory;
                    m_iAllocated = Other.m_iAllocated;
                    m_iSize = Other.m_iSize;
                    
                    Other.m_pMemory = NULL;
                    Other.m_iAllocated = 0;
                    Other.m_iSize = 0;
                }
                return *this;
            }

            void SetArray(const value_t* pData, size_t iLength) {
                this->Clear();

                m_iAllocated = iLength;
                m_iSize = iLength;

                if (iLength > 0) {
                    m_pMemory = alloc_t::Malloc(iLength);
                    for (size_t i = 0; i < iLength; ++i) {
                        new (&m_pMemory[i]) value_t(pData[i]);
                    }
                }
            }
            
            value_t* At(size_t iIndex) {
                if(iIndex >= m_iSize) { return NULL; }
                return &m_pMemory[iIndex];
            }
            
            const value_t* At(size_t iIndex) const {
                if(iIndex >= m_iSize) { return NULL; }
                return (const value_t*)(&m_pMemory[iIndex]);
            }
            
            value_t& operator[](size_t iIndex) {
                return m_pMemory[iIndex];
            }

            const value_t& operator[](size_t iIndex) const {
                return m_pMemory[iIndex];
            }
            
            value_t& Last() {
                return m_pMemory[m_iSize - 1];
            }
            
            const value_t& Last() const {
                return m_pMemory[m_iSize - 1];
            }
            
            void Shrink() {
                if(m_pMemory == NULL) { return; }
                m_pMemory = alloc_t::Realloc(m_pMemory, m_iSize);
            }
            
            void Clear() {
                for (size_t i = 0; i < m_iSize; ++i) {
                    m_pMemory[i].~value_t();
                }
                m_iSize = 0;
            }
            
            size_t PushBack(const value_t& Value) {
                if(m_iSize >= m_iAllocated) {
                    Grow();
                }

                new(&m_pMemory[m_iSize]) value_t(Value);

                return m_iSize++;
            }
            
            size_t PushBack(value_t&& Value) {
                if(m_iSize >= m_iAllocated) {
                    Grow();
                }

                if constexpr( std::is_constructible_v<value_t, value_t&&> ) {
                    new(&m_pMemory[m_iSize]) value_t(std::move(Value));
                } else {
                    memcpy(&m_pMemory[m_iSize], &Value, sizeof(Value));
                }

                memset(&Value, 0, sizeof(Value)); // Reset the moved object

                return m_iSize++;
            }

            size_t PushEmpty() {
                if (m_iSize >= m_iAllocated) {
                    Grow();
                }
                new (&m_pMemory[m_iSize]) value_t();
                return m_iSize++;
            }
            
            void PopBack() {
                if (m_iSize > 0) {
                    m_pMemory[m_iSize - 1].~value_t();
                    --m_iSize;
                }
            }
            
            void InsertAt(size_t iPos, value_t&& Value) {
                if(m_iSize >= m_iAllocated) {
                    Grow();
                }

                if(iPos > m_iSize) { return; }

                m_iSize++;

                if(m_iSize > 0) {
                    // Shift everyone to free said index
                    for(size_t i = m_iSize - 1; i > iPos; --i) {
                        memcpy(&m_pMemory[i], &m_pMemory[i-1], sizeof(value_t));
                    }
                }

                // Move the value
                memcpy(&m_pMemory[iPos], &Value, sizeof(value_t));

                memset(&Value, 0, sizeof(Value)); // Reset the original object we have moved
            }
            
            void InsertAt(size_t iPos, const value_t& Value) {
                if(m_iSize >= m_iAllocated) {
                    Grow();
                }

                if(iPos > m_iSize) { return; }

                if(m_iSize > 0) {
                    for(size_t i = m_iSize - 1; i > iPos; --i) {
                        memcpy(&m_pMemory[i], &m_pMemory[i-1], sizeof(value_t));
                    }
                }

                m_iSize++;

                if constexpr(std::is_constructible_v<value_t, const value_t&>) {
                    new(&m_pMemory[iPos]) value_t(Value);
                } else {
                    memcpy(&m_pMemory[iPos], &Value, sizeof(Value));
                }
            }
            
            void RemoveAt(size_t iPos) {                
                m_pMemory[iPos].~value_t();

                for (size_t i = iPos; i + 1 < m_iSize; ++i) {
                    memcpy(&m_pMemory[i], &m_pMemory[i+1], sizeof(value_t));
                    m_pMemory[i + 1].~value_t();
                }
                
                --m_iSize;
            }
            
            size_t Find(const value_t& value_target) const {
                for (size_t i = 0; i < m_iSize; ++i) {
                    if (m_pMemory[i] == value_target) {
                        return i;
                    }
                }
                return (size_t)(-1);
            }
            
            value_t* Data() { return m_pMemory; }
            const value_t* Data() const { return m_pMemory; }
            
            size_t Length() const { return m_iSize; }
            size_t Capacity() const { return m_iAllocated; }
            
            void Reserve(size_t iNewCapacity) {
                if (iNewCapacity > m_iAllocated) {
                    m_pMemory = alloc_t::Realloc(m_pMemory, iNewCapacity);
                    m_iAllocated = iNewCapacity;
                }
            }
            
        private:
            value_t* m_pMemory = NULL;
            size_t m_iAllocated = 0;
            size_t m_iSize = 0;
            
            void Grow() {
                size_t iNewAllocated = m_iAllocated == 0 ? 1 : m_iAllocated * 2;
                Reserve(iNewAllocated);
            }
    };
}

#endif
