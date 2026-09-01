#ifndef HOTCHLIB_ALLOCATOR_API_H
#define HOTCHLIB_ALLOCATOR_API_H

#include <stddef.h>

namespace hlib {
    class IAllocator {
        public:
            virtual void OnPanic() = 0;

            virtual void* Malloc(size_t iBytes, size_t iAlign = 16) = 0;
            virtual void* Realloc(void* pData, size_t iBytes, size_t iAlign = 16) = 0;
            virtual void Free(void* pData) = 0;
    };

    class CDefaultAllocator final : public IAllocator {
        public:
            void OnPanic() override;

            void* Malloc(size_t iBytes, size_t iAlign = 16) override;
            void* Realloc(void* pData, size_t iBytes, size_t iAlign = 16) override;
            void Free(void* pData) override;
    };

    extern CDefaultAllocator g_defaultAlloc;
}

#endif
