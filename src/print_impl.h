#ifndef HOTCHLIB_SRC_PRINT_IMPL_H
#define HOTCHLIB_SRC_PRINT_IMPL_H

#include "Filesystem.h"

namespace hlib {
    inline size_t PrintImpl(hlib::IFile* stream, const char* data) {
        size_t len = stream->Write((void*)data, strlen(data));
        return (len > 0) ? len : 0;
    }

    inline size_t WriteChars(IFile* stream, const char* data, size_t size) {
        return stream->Write((void*)data, size);
    }

    inline size_t PrintImpl(IFile* stream, int value) {
        static char buff[32];
        size_t k = snprintf(buff, sizeof(buff), "%d", value);
        return WriteChars(stream, buff, sizeof(buff));
    }

    inline size_t PrintImpl(IFile* stream, double value) {
        static char buff[32];
        size_t k = snprintf(buff, sizeof(buff), "%f", value);
        return WriteChars(stream, buff, sizeof(buff));
    }

    inline size_t PrintImpl(IFile* stream, float value) {
        return PrintImpl(stream, (double)value);
    }
}

#endif
