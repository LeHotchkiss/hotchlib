#ifndef HOTCHLIB_API_STRING_API_H
#define HOTCHLIB_API_STRING_API_H

/*
    String facilities.
    All data is expected to be encoded in UTF-8 or plain ASCII
*/

#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>

#include "hlib/Allocator.h"

namespace hlib {
    typedef const char* cstring_t;

    /*
        Constexpr versions for some of <string.h> functions.
        For non-constexpr usage better use standart C calls.
    */

    constexpr void MemCopy(char* pDest, const char* pSource, size_t iLen) {
        for(size_t i = 0; i < iLen; i++) {
            *(pDest + i) = *(pSource + i);
        }
    }

    constexpr void MemSet(char* pDest, char iByte, size_t iLen) {
        for(size_t i = 0; i < iLen; i++) {
            *(pDest + i) = iByte;
        }
    }

    constexpr size_t StringLen(const char* sData) {
        for(size_t i = 0;;i++) {
            if(*(sData + i) == '\0') {
                return i;
            }
        }
    }

    constexpr int StringCmp(const char* sFirst, const char* sSecond) {
        while (*sFirst != '\0' && *sFirst == *sSecond) {
            ++sFirst;
            ++sSecond;
        }

        return (int)*sFirst - (int)*sSecond;
    }

    constexpr int StringCmp(const char* sFirst, const char* sSecond, size_t iMaxChars) {
        for(size_t i = 0; i < iMaxChars; i++) {
            const char c1 = *sFirst;
            const char c2 = *sSecond;

            if(c1 == '\0' || c1 != c2) {
                return (int)c1 - (int)c2;
            }

            ++sFirst; ++sSecond;
        }

        return 0;
    }

    constexpr const char* StringChr(const char* sHay, char iNeedle) {
        while(*sHay != '\0') {
            if(*sHay == iNeedle) { return sHay; }

            ++sHay;
        }

        return NULL;
    }

    constexpr const char* StringStr(const char* sHay, const char* sNeedle) {
        const size_t iNeedleLen = StringLen(sNeedle);

        while(*sHay != '\0') {
            if( StringCmp(sHay, sNeedle, iNeedleLen) == 0 ) {
                return sHay;
            }

            ++sHay;
        }

        return NULL;
    }

    /*
        Constant compile-time string wrapper.
        Has operators overloaded and can be used in containers.
    */
    class CConstString {
        const char* m_sData = NULL;
        public:
            constexpr CConstString(const char* sData) : m_sData(sData) {}

            constexpr bool operator<(const char* sOther) const { return StringCmp(m_sData, sOther) < 0; }
            constexpr bool operator==(const char* sOther) const { return StringCmp(m_sData, sOther) == 0; }
            constexpr bool operator>(const char* sOther) const { return StringCmp(m_sData, sOther) > 0; }

            constexpr size_t Length() const { return StringLen(m_sData); }

            constexpr const char* String() const { return m_sData; }
            constexpr operator cstring_t() const { return m_sData; }

            constexpr operator bool() const { return m_sData != NULL; }

            constexpr const char* Find(const char* sSubstr) const {
                return StringStr(m_sData, sSubstr);
            }

            constexpr const char* Find(char iChar) const {
                return StringChr(m_sData, iChar);
            }
    };
    
    /*
        A regular string. Has it`s length cached, so if you
        have done something nasty with the data you must recache it
        with LengthUpdate() call, or something horrible might happen.

        Still NULL-terminated tho.
    */
    template <typename alloc_t, alloc_t* t_pAlloc> class CString {
        private:
            constexpr static const char* s_sNullString = "(null)";
            static char s_sDummy;

            char* m_sData = NULL;
            size_t m_iLength = 0;

            void Nullify() {        
                m_sData = (char*)t_pAlloc->Realloc(m_sData, 1);
                m_sData[0] = '\0';
                
                m_iLength = 0;
            }

            char* Malloc(size_t iSize) {
                return (char*)t_pAlloc->Malloc(iSize, alignof(char));
            }

            char* Realloc(char* pData, size_t iSize) {
                return (char*)t_pAlloc->Realloc(pData, iSize, alignof(char));
            }

            void Free(char* pData) {
                t_pAlloc->Free(pData);
            }

            void PushString(const char* sData, size_t iLen) {
                const size_t iOldLength = m_iLength;
                const size_t iNewLength = iOldLength + iLen;

                m_sData = this->Realloc(
                    m_sData,
                    iNewLength + 1
                );

                memcpy(
                    m_sData + iOldLength,
                    sData,
                    iLen
                );

                m_iLength = iNewLength;
                m_sData[m_iLength] = '\0';
            }

        public:
            CString() { this->Nullify(); }

            CString(size_t iLength) {
                m_iLength = iLength;
                m_sData = this->Malloc(iLength+1);
                m_sData[iLength] = '\0';
            }

            CString(const char* sSource) {
                if (sSource == NULL) {
                    this->Nullify();
                    return;
                }
                
                m_iLength = strlen(sSource);
                m_sData = this->Malloc(m_iLength + 1);

                memcpy(m_sData, sSource, m_iLength);
                m_sData[m_iLength] = '\0';
            }

            CString(const CString& Other) {
                if (!Other.IsValid()) {
                    Nullify();
                    return;
                }
                
                m_iLength = Other.m_iLength;
                m_sData = this->Malloc(m_iLength + 1);
                memcpy(m_sData, Other.m_sData, m_iLength + 1);
            }
            
            CString(CString&& Other) : m_sData(Other.m_sData), m_iLength(Other.m_iLength) {
                Other.m_sData = NULL;
                Other.m_iLength = 0;
            }

            ~CString() {
                if (m_sData != NULL) {
                    this->Free(m_sData);
                    m_sData = NULL;
                }
            }
            
            CString& operator=(const char* sOther) {
                if (sOther == NULL) {
                    this->Nullify();
                    return *this;
                }

                const size_t iLength = strlen(sOther);
                m_sData = this->Realloc(m_sData, iLength + 1);
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
                
                m_sData = this->Realloc(m_sData, Other.m_iLength+1);
                memcpy(m_sData, Other.m_sData, Other.m_iLength+1);

                m_iLength = Other.m_iLength;
                
                return *this;
            }

            CString& operator=(CString&& Other) {
                if (this == &Other) {
                    return *this;
                }
                
                if (m_sData != NULL) {
                    this->Free(m_sData);
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

            void Clear() {
                Nullify();
            }

            void Set(const CString& sData) {
                m_iLength = sData.m_iLength;

                m_sData = this->Realloc(m_sData, m_iLength+1);
                m_sData[m_iLength] = '\0';

                memcpy(m_sData, sData.String(), m_iLength);
            }

            void Set(const char* sData) {
                m_iLength = strlen(sData);

                m_sData = this->Realloc(m_sData, m_iLength+1);
                m_sData[m_iLength] = '\0';

                memcpy(m_sData, sData, m_iLength);
            }

            /* 
                Resize string to host iLength characters.
                NULL-terminator is added at the end and does
                not occupy the reserved space
            */
            void Resize(size_t iLength) {
                if(iLength == m_iLength) {
                    return;
                }

                m_sData = this->Realloc(m_sData, iLength+1);

                if(iLength > m_iLength) {
                    memset(m_sData + m_iLength, iLength-m_iLength, ' ');
                }

                m_sData[iLength] = '\0';
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

            CString& operator+=(const char* pOther) {
                this->PushString(pOther, strlen(pOther));
                
                return *this;
            }

            CString& operator+=(const CString& Other) {
                this->PushString(Other.String(), Other.Length());
                
                return *this;
            }

            CString& operator+=(char iOther) {
                m_sData = this->Realloc(m_sData, m_iLength + 1);
                m_sData[m_iLength] = iOther;
                m_sData[m_iLength+1] = '\0';

                m_iLength++;
                
                return *this;
            }
            
            size_t Printv(const char* sFormat, va_list Args) {
                va_list argsCopy;
                va_copy(argsCopy, Args);

                size_t iLen = vsnprintf(m_sData, 0, sFormat, argsCopy);

                va_end(argsCopy);

                this->Resize(iLen+1);
                return vsnprintf(m_sData, m_iLength, sFormat, Args);
            }
            
            size_t Printf(const char* sFormat, ...) {
                va_list Args;
                va_start(Args, sFormat);

                va_list argsCopy;
                va_copy(argsCopy, Args);

                size_t iLen = vsnprintf(m_sData, 0, sFormat, Args);

                va_end(argsCopy);

                this->Resize(iLen+1);

                size_t iLn = vsnprintf(m_sData, m_iLength, sFormat, Args);

                va_end(Args);

                return iLn;
            }

            size_t PushFormattedv(const char* sFormat, va_list Args) {
                va_list argsCopy;
                va_copy(argsCopy, Args);

                const size_t iLen = vsnprintf(NULL, 0, sFormat, argsCopy);

                va_end(argsCopy);

                m_sData = this->Realloc(
                    m_sData,
                    m_iLength + iLen + 1
                );

                vsnprintf(
                    m_sData + m_iLength,
                    iLen + 1,
                    sFormat,
                    Args
                );

                m_iLength += iLen;
                m_sData[m_iLength] = '\0';

                return iLen;
            }
            
            size_t PushFormatted(const char* sFormat, ...) {
                va_list args;
                va_start(args, sFormat);

                va_list argsCopy;
                va_copy(argsCopy, args);

                const size_t iLen = vsnprintf(NULL, 0, sFormat, argsCopy);

                va_end(argsCopy);

                char* newData = this->Realloc(
                    m_sData,
                    m_iLength + iLen + 1
                );

                if (!newData) {
                    va_end(args);
                    return 0;
                }

                m_sData = newData;

                vsnprintf(
                    m_sData + m_iLength,
                    iLen + 1,
                    sFormat,
                    args
                );

                m_iLength += iLen;
                m_sData[m_iLength] = '\0';

                va_end(args);

                return iLen;
            }
            
            operator cstring_t() const { return (const char*)m_sData; }
            operator char*() { return m_sData; }
            operator bool() const { return this->IsValid(); }
    };

    using string_t = CString<CDefaultAllocator, &g_defaultAlloc>;

    /* 
        Get current time formatted in a string buffer.
        Returns the amount of bytes written
    */
    size_t StringFormatTime(char* sBuffer, size_t iLength, const char* sFormat);

    /* 
        Get specified time formatted in a string buffer.
        Returns the amount of bytes written
    */
    size_t StringFormatTime(char* sBuffer, size_t iLength, const char* sFormat, time_t iTimer);

    /*
        -1 - not a number
        0  - integer
        1  - float 
    */
    int32_t IsNumber(const char* sString);
}

#endif
