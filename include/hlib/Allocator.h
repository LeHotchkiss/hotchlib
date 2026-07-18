#ifndef CBPP_ALLOCATOR_API_H
#define CBPP_ALLOCATOR_API_H

#include <stddef.h>
#include <stdlib.h>

namespace hlib {
    template <typename value_t> class CDefaultAllocator {
        public:
            static value_t* Malloc(size_t iCount) {
                return (value_t*)malloc( sizeof(value_t) * iCount );
            }

            static value_t* Realloc(value_t* pData, size_t iNewLen) {
                return (value_t*)realloc(pData, iNewLen*sizeof(value_t));
            }

            static void Free(value_t* pData) {
                free(pData);
            }
    };
}

#endif
