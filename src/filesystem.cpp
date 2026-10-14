#include "Filesystem.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "Bit.h"
#include "New.h"
#include "System.h"

#if defined(HLIB_LINUX)
    #include <sys/stat.h>
#elif defined(HLIB_WINDOWS)
    #include <windows.h>
#endif

namespace hlib {
    class CFile : public IFile {
        FILE* m_pFile = NULL;
        public:
            bool Open(const char* sPath, int iModes) override {
                char modes[16] = {'\0'};

                if( TestBitmask<int>(iModes, EFileMode::Read) ) {
                    strcat(modes, "r");
                }

                if( TestBitmask<int>(iModes, EFileMode::Write) ) {
                    strcat(modes, "w");
                }

                if( TestBitmask<int>(iModes, EFileMode::Append) ) {
                    strcat(modes, "a");
                }

                strcat(modes, "b");

                m_pFile = fopen64(sPath, modes);

                return m_pFile != NULL;
            }

            void Close() override {
                fclose(m_pFile);
            }

            int GetChar() const override {
                return (int)fgetc(m_pFile);
            }

            void PutChar(int c) override {
                fputc(c, m_pFile);
            }

            size_t Printf(const char* format, ...) override {
                va_list args;
                va_start(args, format);
                size_t k = (size_t)vfprintf(m_pFile, format, args);
                va_end(args);

                return k;
            }

            size_t Scanf(const char* format, ...) override {
                va_list args;
                va_start(args, format);
                size_t k = vfscanf(m_pFile, format, args);
                va_end(args);

                return k;
            }

            void SetPos(EFilePos pos, size_t amount) override {
                fsetpos64(m_pFile, (fpos64_t*)&amount);
            }

            size_t GetPos() const override {
                fpos64_t pos;
                fgetpos64(m_pFile, &pos);

                return *(size_t*)(&pos);
            }

            void Rewind() override {
                rewind(m_pFile);
            }

            bool IsEOF() const override {
                return feof(m_pFile) != 0;
            }

            size_t Write(void* data, size_t bytes) override {
                return fwrite(data, bytes, 1, m_pFile);
            }

            size_t Read(void* data, size_t bytes) override {
                return fread(data, bytes, 1, m_pFile);
            }
    };

    IFile* FileOpen(const char* path, int mode) {
        CFile* file = New<CFile>();

        if(!file->Open(path, mode)) {
            return NULL;
        }

        return (IFile*)file;
    }

    void FileClose(IFile* file) {
        file->Close();
        Delete<CFile>((CFile*)file);
    }

    #if defined(HLIB_LINUX)
    SFileInfo GetFileInfo(const char* path) {
        struct stat64 info;
        SFileInfo out;

        int code = stat64(path, &info);
        if(code != 0) {
            return out;
        }

        out.exists = true;
        out.size = info.st_size;

        out.accessTime.secs = info.st_atim.tv_sec;
        out.accessTime.nsecs = info.st_atim.tv_nsec;
        out.modifTime.secs = info.st_mtim.tv_sec;
        out.modifTime.nsecs = info.st_mtim.tv_nsec;

        return out;
    }
    #endif

    #if defined(HLIB_WINDOWS)
        static_assert(false, "W*ndows version is not implemented");
    #endif
}
