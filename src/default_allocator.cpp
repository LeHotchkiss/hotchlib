#include "Allocator.h"
#include "hlib/System.h"

#include <stdlib.h>
#include <string.h>
#include <malloc.h>

namespace hlib {

    CDefaultAllocator g_defaultAlloc;

    void* CDefaultAllocator::Malloc(size_t iBytes, size_t iAlign) {
        return aligned_alloc(iAlign, iBytes);
    }

    #if defined(HLIB_WINDOWS) 
    void* CDefaultAllocator::Realloc(void* pData, size_t iBytes, size_t iAlign) {
        return _aligned_realloc(pData, iBytes, iAlign);
    }
    #endif

    #if defined(HLIB_LINUX)
    void* CDefaultAllocator::Realloc(void* pData, size_t iBytes, size_t iAlign) {
        if ((iBytes == 0) || (iAlign <= alignof(max_align_t))) {
            return realloc(pData, iBytes);
        }
        
        size_t iNewSize = (iBytes + (iAlign - 1)) & (~(iAlign - 1));
        
        void *pNew = aligned_alloc(iAlign, iNewSize);
        if (pNew != NULL) {
            size_t iOldSize = malloc_usable_size(pData);
            size_t iCopySize = iNewSize < iOldSize ? iNewSize : iOldSize;

            if (pData != NULL) {
                memcpy(pNew, pData, iCopySize);
                free(pData);
            }
        }
        
        return pNew;
    }
    #endif

    void CDefaultAllocator::Free(void* pData) {
        free(pData);
    }
}
