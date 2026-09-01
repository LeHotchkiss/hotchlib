#include "String.h"

#include <stdlib.h>

namespace hlib {
    constexpr CConstString::CConstString(const char* sData) : m_sData(sData) {}

    constexpr bool CConstString::operator<(const char* sOther) const { return StringCmp(m_sData, sOther) < 0; }
    constexpr bool CConstString::operator==(const char* sOther) const { return StringCmp(m_sData, sOther) == 0; }
    constexpr bool CConstString::operator>(const char* sOther) const { return StringCmp(m_sData, sOther) > 0; }

    constexpr size_t CConstString::Length() const { return StringLen(m_sData); }

    constexpr const char* CConstString::String() const { return m_sData; }
    constexpr CConstString::operator cstring_t() const { return m_sData; }

    constexpr CConstString::operator bool() const { return m_sData != NULL; }

    constexpr const char* CConstString::Find(const char* sSubstr) const {
        return StringStr(m_sData, sSubstr);
    }

    constexpr const char* CConstString::Find(char iChar) const {
        return StringChr(m_sData, iChar);
    }
}

namespace hlib {
    size_t StringFormatTime(char* sBuffer, size_t iLength, const char* sFormat) {
        time_t iTime = time(NULL);
        return strftime( sBuffer, iLength, sFormat, localtime(&iTime) );
    }

    size_t StringFormatTime(char* sBuffer, size_t iLength, const char* sFormat, time_t iTimer) {
        return strftime( sBuffer, iLength, sFormat, localtime(&iTimer) );
    }

    int32_t IsNumber(const char* sString) {
        char *pEnd;
        float val = strtof(sString, &pEnd);

        if (*pEnd == '\0') {
            if (strchr(sString, '.') == NULL && strchr(sString, 'e') == NULL && strchr(sString, 'E') == NULL) {
                return 0;
            } else {
                return 1;
            }
        }

        return -1;
    }
}
