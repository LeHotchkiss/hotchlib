#include "hlib/Allocator.h"

#include <stdlib.h>

namespace hlib {
    void* CDefaultAllocator::Malloc(size_t iBytes) {
        return malloc(iBytes);
    }

    void* CDefaultAllocator::Realloc(void* pData, size_t iBytes) {
        return realloc(pData, iBytes);
    }

    void CDefaultAllocator::Free(void* pData) {
        free(pData);
    }
}
