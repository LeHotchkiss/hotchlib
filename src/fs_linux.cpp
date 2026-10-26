#include "System.h"

#ifdef HLIB_LINUX

#include "Filesystem.h"
#include "Bit.h"
#include "New.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace hlib {
    IFile *InStream, *OutStream;

    class CFile : public IFile {
        int m_fd = -1;
        public:
            bool Open(const char* sPath, int iModes) override {
                const bool read     = TestBitmask<int>(iModes, EFileMode::Read);
                const bool write    = TestBitmask<int>(iModes, EFileMode::Write);
                const bool append   = TestBitmask<int>(iModes, EFileMode::Append);

                if (!read && !write) return false;
                if (append && !write) return false;

                int flags = read && write ? O_RDWR : (write ? O_WRONLY : O_RDONLY);

                if (write) {
                    flags |= O_CREAT;
                    flags |= append ? O_APPEND : O_TRUNC;
                }

                int fd = open64(sPath, flags, 0644);
                if (fd == -1) { return false; }

                if (m_fd >= 0) { close(m_fd); }
                m_fd = fd;
                return true;
            }
            
            void Close() override {
                if(m_fd > 0) {
                    close(m_fd);
                    m_fd = -1;
                }
            }
            
            int GetChar() const override {
                unsigned char c;

                int result = read(m_fd, &c, 1);

                if(result <= 0) {
                    return -1;
                }

                return c;
            }
            
            void PutChar(int c) override {
                unsigned char byte = c;
                write(m_fd, &byte, 1);
            }

            void SetPos(EFilePos pos, int64_t amount) override {
                int position;
                switch(pos) {
                    case EFilePos::Current: 
                        position = SEEK_CUR; break;

                    case EFilePos::End:
                        position = SEEK_END; break;

                    case EFilePos::Start:
                        position = SEEK_SET; break;
                }

                lseek64(m_fd, amount, position);
            }
            
            size_t GetPos() const override {
                return lseek64(m_fd, SEEK_CUR, 0);
            }

            void Rewind() override {
                lseek64(m_fd, SEEK_SET, 0);
            }

            size_t Write(void* data, size_t bytes) override {
                return (size_t)write(m_fd, data, bytes);
            }

            size_t Read(void* data, size_t bytes) override {
                return (size_t)read(m_fd, data, bytes);
            }
    };

    class CInStream : public IFile {
        public:
            bool Open(const char* sPath, int iModes) override {
                return false;
            }

            void Close() override {
                return;
            }
            
            int GetChar() const override {
                unsigned char c;

                int result = read(STDIN_FILENO, &c, 1);

                if(result <= 0) {
                    return -1;
                }

                return c;
            }

            void PutChar(int c) override {
                return;
            }

            void SetPos(EFilePos pos, int64_t amount) override {
                return;
            }
            
            size_t GetPos() const override {
                return 0;
            }

            void Rewind() override {
                return;
            }

            size_t Write(void* data, size_t bytes) override {
                return 0;
            }

            size_t Read(void* data, size_t bytes) override {
                return (size_t)read(STDIN_FILENO, data, bytes);
            }
    };

    class COutStream : public IFile {
        public:
            bool Open(const char* sPath, int iModes) override {
                return false;
            }

            void Close() override {
                return;
            }
            
            int GetChar() const override {
                return -1;
            }

            void PutChar(int c) override {
                unsigned char byte = c;
                write(STDOUT_FILENO, &byte, 1);
            }

            void SetPos(EFilePos pos, int64_t amount) override {
                return;
            }
            
            size_t GetPos() const override {
                return 0;
            }

            void Rewind() override {
                return;
            }

            size_t Write(void* data, size_t bytes) override {
                return (size_t)write(STDOUT_FILENO, data, bytes);
            }

            size_t Read(void* data, size_t bytes) override {
                return 0;
            }
    };

    SFileInfo GetFileInfo(const char* path) {
        struct stat64 info;
        SFileInfo out = {0};

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

    static class CStreamsRegistrator {
        CInStream m_in;
        COutStream m_out;

        public:
            CStreamsRegistrator() {
                InStream = &m_in;
                OutStream = &m_out;
            }
    } g_registrator;

    IFile* FileOpen(const char* path, int mode) {
        CFile* file = New<CFile>();

        if(!file->Open(path, mode)) {
            Delete<CFile>(file);
            return NULL;
        }

        return (IFile*)file;
    }

    void FileClose(IFile* file) {
        file->Close();
        Delete<CFile>((CFile*)file);
    }
}

#endif
