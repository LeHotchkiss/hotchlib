#ifndef HOTCHLIB_ALLOCATOR_API_H
#define HOTCHLIB_ALLOCATOR_API_H

#include <stddef.h>

namespace hlib {
    class CDefaultAllocator {
        public:
            static void* Malloc(size_t iBytes);
            static void* Realloc(void* pData, size_t iNewLen);
            static void Free(void* pData);
    };
}

#endif
