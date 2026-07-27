#ifndef HOTCHLIB_API_STRING_API_H
#define HOTCHLIB_API_STRING_API_H

/*
    String facilities.
    All data is expected to be encoded in UTF-8 or plain ASCII
*/

#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <stdarg.h>

#include "hlib/Allocator.h"

namespace hlib {
    typedef const char* cstring_t;

    /*
        Constant compile-time string wrapper.
        Has operators overloaded and can be used in containers.
    */
    class CConstString {
        const char* m_sData;
        public:
            CConstString();
            CConstString(const char* sData);

            bool operator<(const char* sOther) const;
            bool operator==(const char* sOther) const;
            bool operator>(const char* sOther) const;

            size_t Length() const;

            const char* String() const;
            operator cstring_t() const;

            operator bool() const;
    };

    /*
        A regular string. Has it`s length cached, so if you
        have done something nasty with the data you must recache it
        with LengthUpdate() call, or something horrible might happen.

        Still NULL-terminated tho.
    */
    template <typename alloc_t = CDefaultAllocator<unsigned char>> class CString {
        private:
            constexpr static const char* s_sNullString = "(null)";
            static char s_sDummy;

            char* m_sData = NULL;
            size_t m_iLength = 0;

            void Nullify() {        
                m_sData = alloc_t::Realloc(m_sData, 1);
                m_sData[0] = '\0';
                
                m_iLength = 0;
            }
            
        public:
            CString() { this->Nullify(); }

            CString(size_t iLength) {
                m_iLength = iLength;
                m_sData = alloc_t::Malloc(iLength+1);
                m_sData[iLength] = '\0';
            }

            CString(const char* sSource) {
                if (sSource == NULL) {
                    this->Nullify();
                    return;
                }
                
                m_iLength = strlen(sSource);
                m_sData = alloc_t::Malloc(m_iLength + 1);

                memcpy(m_sData, sSource, m_iLength);
                m_sData[m_iLength] = '\0';
            }

            CString(const CString& Other) {
                if (!Other.IsValid()) {
                    Nullify();
                    return;
                }
                
                m_iLength = Other.m_iLength;
                m_sData = alloc_t::Malloc(m_iLength + 1);
                memcpy(m_sData, Other.m_sData, m_iLength + 1);
            }

            CString(CString&& Other) : m_sData(Other.m_sData), m_iLength(Other.m_iLength) {
                Other.m_sData = NULL;
                Other.m_iLength = 0;
            }

            ~CString() {
                if (m_sData != NULL) {
                    alloc_t::Free(m_sData);
                    m_sData = NULL;
                }
            }
            
            CString& operator=(const char* sOther) {
                if (sOther == NULL) {
                    this->Nullify();
                    return *this;
                }

                const size_t iLength = strlen(sOther);
                m_sData = alloc_t::Realloc(m_sData, iLength + 1);
                memcpy(m_sData, sOther, iLength);
                m_sData[iLength] = '\0';

                m_iLength = iLength;
                
                return *this;
            }

            CString& operator=(const CString& Other) {
                if (this == &Other) {
                    return *this;
                }
                
                if (!Other.IsValid()) {
                    this->Nullify();
                    return *this;
                }
                
                m_sData = alloc_t::Realloc(m_sData, Other.m_iLength+1);
                memcpy(m_sData, Other.m_sData, Other.m_iLength+1);

                m_iLength = Other.m_iLength;
                
                return *this;
            }

            CString& operator=(CString&& Other) {
                if (this == &Other) {
                    return *this;
                }
                
                if (m_sData != NULL) {
                    alloc_t::Free(m_sData);
                }
                
                m_sData = Other.m_sData;
                m_iLength = Other.m_iLength;
                
                Other.m_sData = NULL;
                Other.m_iLength = 0;
                
                return *this;
            }
            
            const char* String() const { return m_sData != NULL ? m_sData : s_sNullString; }
            char* Pointer() { return m_sData; }

            /*
                Length in bytes, including the NULL-terminator
            */
            size_t Size() const { return m_iLength+1; }
            
            /*
                Length in bytes, excluding the NULL-terminator
            */
            size_t Length() const { return m_iLength; }
            
            size_t LengthUpdate() {
                if (m_sData != NULL) {
                    m_iLength = strlen(m_sData);
                } else {
                    m_iLength = 0;
                }
                return m_iLength;
            }

            void Set(const CString& sData) {
                m_iLength = sData.m_iLength;

                m_sData = alloc_t::Realloc(m_sData, m_iLength+1);
                m_sData[m_iLength] = '\0';

                memcpy(m_sData, sData.String(), m_iLength);
            }

            void Set(const char* sData) {
                m_iLength = strlen(sData);

                m_sData = alloc_t::Realloc(m_sData, m_iLength+1);
                m_sData[m_iLength] = '\0';

                memcpy(m_sData, sData, m_iLength);
            }

            // Resize string to host iLength characters
            void Resize(size_t iLength) {
                m_sData = alloc_t::Realloc(m_sData, iLength+1);
                m_sData[iLength+1] = '\0';
                m_iLength = iLength;
            }
            
            bool IsValid() const { return m_sData != NULL && m_sData[0] != '\0'; }
            
            char& operator[](size_t iIndex) { return m_sData[iIndex]; }
            const char& operator[](size_t iIndex) const { return m_sData[iIndex]; }

            char& At(size_t iIndex) {
                if(iIndex > m_iLength) { return s_sDummy; }
                return m_sData[iIndex];
            }

            const char& At(size_t iIndex) const {
                if(iIndex > m_iLength) { return s_sDummy; }
                return m_sData[iIndex];
            }
            
            bool operator==(const char* sOther) const {
                if(m_sData == NULL && sOther != NULL) { return false; }
                return strcmp(m_sData, sOther) == 0;
            }
            
            bool operator<(const char* sOther) const {
                if(m_sData == NULL && sOther != NULL) { return false; }
                return strcmp(m_sData, sOther) < 0;
            }

            bool operator>(const char* sOther) const {
                if(m_sData == NULL && sOther != NULL) { return false; }
                return strcmp(m_sData, sOther) > 0;
            }
            
            CString& operator+=(const CString& Other) {
                if (!Other.IsValid()) {
                    return *this;
                }
                
                const size_t iNewLength = m_iLength + Other.m_iLength + 1,
                            iOldLength = m_iLength;

                m_sData = alloc_t::Realloc(m_sData, iNewLength);
                memcpy(m_sData + iOldLength, Other.m_sData, Other.m_iLength);

                m_iLength = iNewLength;
                
                return *this;
            }

            CString& operator+=(char iOther) {
                m_sData = alloc_t::Realloc(m_sData, m_iLength + 1);
                m_sData[m_iLength] = iOther;
                m_sData[m_iLength+1] = '\0';

                m_iLength++;
                
                return *this;
            }

            size_t Printv(const char* sFormat, va_list Args) {
                return vsnprintf(m_sData, m_iLength, sFormat, Args);
            }
            
            size_t Printf(const char* sFormat, ...) {
                va_list Args;
                va_start(Args, sFormat);
                size_t iLn = vsnprintf(m_sData, m_iLength, sFormat, Args);
                va_end(Args);

                return iLn;
            }
            
            operator cstring_t() const { return (const char*)m_sData; }
            operator char*() { return m_sData; }
            operator bool() const { return this->IsValid(); }
    };

    /*CString operator+(const CString& A, const CString& B);
    CString operator+(const CString& A, const char* B);*/

    /*
        A reference that points to a substring in the source string.
        Useful when working with a single big text to avoid allocations and copying.

        Obviously, this class is useable until referenced string is valid.
    */
    /*class CSubString {
        char *m_pStart = NULL, *m_pEnd = NULL;

        public:
            CSubString(CString&& sSource, size_t iPos, size_t iLength) = delete;

            CSubString() = default;

            CSubString(const CString& sSource, size_t iStart, size_t iLength);
            CSubString(const char* sSource, size_t iStart, size_t iLength);

            void Set(const char* sSource, size_t iStart, size_t iLength);

            bool operator==(const CSubString& Other) const;
            bool operator<(const CSubString& Other) const;
            bool operator>(const CSubString& Other) const;

            size_t Length() const;
            
            char* Start() const;
            char* End() const;

            // Get substring as a new string
            CString Copy() const;

            // Output the string to an external buffer, adding the null-terminator
            size_t Bufferize(char* pBuffer, size_t iBuffSize) const;
    };*/

    /*
        String pool.
        All of them are stored continuously, the 
        memory layout looks like this:

        string1 \0 string2 \0 string3 \0

        'indices' here are byte offsets from the beginning of
        the pool.
    */
    class CStringPool {
        char* m_pData = NULL;
        size_t m_iCount = 0;
        size_t m_iBytes = 0;

        size_t PushString(const char* sData);

        public:
            CStringPool() = default;

            const char* Data() const;

            /*
                Byte length of the allocated memory
            */
            size_t Size() const;

            /*
                Amount of strings in the pool
            */
            size_t Length() const;

            size_t AddOrRef(const char* sData);
            size_t Find(const char* sData) const;

            const char* At(size_t iIndex) const;
            const char* operator[](size_t iIndex) const;

            void Clear();

            ~CStringPool();
    };

    /*
        Works like regular strcmp, but implemented for substrings
    */
    //int32_t SubStringCmp(const CSubString& sA, const CSubString& sB);

    /* 
        Get current time formatted in a string buffer.
        Returns the amount of bytes written
    */
    //size_t StringFormatTime(char* sBuffer, size_t iLength, const char* sFormat);

    /* 
        Get specified time formatted in a string buffer.
        Returns the amount of bytes written
    */
    //size_t StringFormatTime(char* sBuffer, size_t iLength, const char* sFormat, time_t iTimer);

    /*
        -1 - not a number
        0  - integer
        1  - float 
    */
    //int32_t IsNumber(const char* sString);

    #define HOTCHLIB_IMPLEMENTATION

    #ifdef HOTCHLIB_IMPLEMENTATION

    CConstString::CConstString() : m_sData(NULL) {}
    CConstString::CConstString(const char* sData) : m_sData(sData) {}

    bool CConstString::operator<(const char* sOther) const { return strcmp(m_sData, sOther) < 0; }
    bool CConstString::operator==(const char* sOther) const { return strcmp(m_sData, sOther) == 0; }
    bool CConstString::operator>(const char* sOther) const { return strcmp(m_sData, sOther) > 0; }

    size_t CConstString::Length() const { return sizeof(m_sData) - 1; }

    const char* CConstString::String() const { return m_sData; }
    CConstString::operator cstring_t() const { return m_sData; }

    CConstString::operator bool() const { return m_sData != NULL; }

    #endif
}

#endif
