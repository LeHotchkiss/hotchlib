#ifndef HOTCHLIB_API_NEW_H
#define HOTCHLIB_API_NEW_H

#include <type_traits>
#include <utility>
#include <new>

#include "Allocator.h"

namespace hlib {
    // Allocate a new object
    template <typename value_t, typename alloc_t = CDefaultAllocator, alloc_t* t_pAlloc = &g_defaultAlloc, typename... args_t> 
    value_t* New(args_t&&... Args) {
        value_t* pBuffer = (value_t*)t_pAlloc->Malloc(sizeof(value_t));
        new(pBuffer) value_t(std::forward<args_t>(Args)...);
        return pBuffer;
    }

    // Allocate a new array of objects
    template <typename value_t, typename alloc_t = CDefaultAllocator, alloc_t* t_pAlloc = &g_defaultAlloc, typename... args_t>
    value_t* NewA(size_t iAmount, args_t&&... Args) {
        value_t* pBuffer = (value_t*)t_pAlloc->Malloc(sizeof(value_t));
        for(size_t i = 0; i < iAmount; i++) {
            new(&pBuffer[i]) value_t(std::forward<args_t>(Args)...);
        }
        return pBuffer;
    }

    // Properly deallocate an object
    template <typename value_t, typename alloc_t = CDefaultAllocator, alloc_t* t_pAlloc = &g_defaultAlloc> 
    void Delete(value_t* pObject) {
        if constexpr ( std::is_destructible_v<value_t> ) {
            pObject->~value_t();
        }
        t_pAlloc->Free(pObject);
    }

    // Properly deallocate an array of objects
    template <typename value_t, typename alloc_t = CDefaultAllocator, alloc_t* t_pAlloc = &g_defaultAlloc>
    void DeleteA(IAllocator* pAlloc, value_t* pArray, size_t iArraySize) {
        if constexpr ( std::is_destructible_v<value_t> ) {
            for(size_t i = 0; i < iArraySize; i++) {
                pArray[i].~value_t();
            }
        }
        t_pAlloc->Free(pArray);
    }
}

#endif
