#include "String.h"

namespace hlib {
    CConstString::CConstString() : m_sData(NULL) {}
    CConstString::CConstString(const char* sData) : m_sData(sData) {}

    bool CConstString::operator<(const char* sOther) const { return strcmp(m_sData, sOther) < 0; }
    bool CConstString::operator==(const char* sOther) const { return strcmp(m_sData, sOther) == 0; }
    bool CConstString::operator>(const char* sOther) const { return strcmp(m_sData, sOther) > 0; }

    size_t CConstString::Length() const { return sizeof(m_sData) - 1; }

    const char* CConstString::String() const { return m_sData; }
    CConstString::operator cstring_t() const { return m_sData; }

    CConstString::operator bool() const { return m_sData != NULL; }
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
