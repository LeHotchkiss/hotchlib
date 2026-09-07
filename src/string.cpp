#include "String.h"

#include <stdlib.h>

namespace hlib {
    
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
