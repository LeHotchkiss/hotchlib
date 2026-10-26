#ifndef HOTCHLIB_API_PRINT_H
#define HOTCHLIB_API_PRINT_H

#include <string.h>

#include "Filesystem.h"
#include "print_impl.h"

namespace hlib {
    template <typename... args_t> 
    size_t Print(IFile* stream, const args_t&... args) {
        size_t bytes = 0;
        ((bytes += PrintImpl(stream, args)), ...);
        return bytes;
    }
}

#endif
