#ifndef HOTCHLIB_SYSTEM_API
#define HOTCHLIB_SYSTEM_API

#if defined(__linux__)
    #define HLIB_LINUX
#elif defined(_WIN32) || defined(_WIN64)
    #define HLIB_WINDOWS
#else
    #error "Undefined OS"
#endif

#endif
