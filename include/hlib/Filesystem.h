#ifndef HOTCHLIB_API_FILESYSTEM_H
#define HOTCHLIB_API_FILESYSTEM_H

#include <stddef.h>
#include <stdint.h>

#include "hlib/Array.h"
#include "hlib/String.h"

namespace hlib {
    /*
        Mimic the enum behaviour, because Retard++ can`t perform implicit
        casts from enum type to integer to allow bitmask combining via "|".
    */

    namespace EFileMode {
        constexpr static const int Read   = 1 << 0;
        constexpr static const int Write  = 1 << 1;
        constexpr static const int Append = 1 << 2;
    }

    enum class EFilePos : int {
        Set,
        Start,
        End
    };

    class IFile {
        public:
            IFile() = default;
            ~IFile() = default;

            virtual bool Open(const char*, int) = 0;
            virtual void Close() = 0;

            virtual int GetChar() const = 0;
            virtual void PutChar(int) = 0;

            virtual size_t Printf(const char*, ...) = 0;
            virtual size_t Scanf(const char*, ...) = 0;

            virtual void SetPos(EFilePos, size_t) = 0;
            virtual size_t GetPos() const = 0;
            virtual void Rewind() = 0;

            virtual bool IsEOF() const = 0;

            virtual size_t Write(void*, size_t) = 0;
            virtual size_t Read(void*, size_t) = 0;
    };

    IFile* FileOpen(const char*, int);
    void FileClose(IFile*);

    struct SDirEntry {
        bool isFile;
        string_t path;
    };

    struct SFileInfo {
        bool exists = false;
        uint64_t size = 0;
        
        struct {
            uint64_t secs;
            uint64_t nsecs;
        } accessTime, modifTime;
    };

    using CDirContent = array_t<SDirEntry>;

    void GetDirContents(const char*, CDirContent&);

    SFileInfo GetFileInfo(const char*);
}

#endif
