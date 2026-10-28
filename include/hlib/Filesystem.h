#ifndef HOTCHLIB_API_FILESYSTEM_H
#define HOTCHLIB_API_FILESYSTEM_H

#include <stddef.h>
#include <stdint.h>

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
        Current,
        Start,
        End
    };

    struct SFileInfo {
        bool exists = false;
        uint64_t size = 0;
        
        struct {
            uint64_t secs;
            uint64_t nsecs;
        } accessTime, modifTime;
    };
    
    class IFile {
        public:
            IFile() = default;
            ~IFile() = default;

            virtual bool Open(const char*, int) = 0;
            virtual void Close() = 0;

            virtual int GetChar() const = 0;
            virtual void PutChar(int) = 0;

            virtual void SetPos(EFilePos, int64_t) = 0;
            virtual size_t GetPos() const = 0;
            virtual void Rewind() = 0;

            virtual size_t Write(void*, size_t) = 0;
            virtual size_t Read(void*, size_t) = 0;

            virtual SFileInfo GetInfo() const = 0;
    };

    extern IFile *OutStream, *InStream;

    IFile* FileOpen(const char*, int);
    void FileClose(IFile*);
}

#endif
