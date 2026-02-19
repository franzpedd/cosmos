#pragma once

#include <stdint.h>

/// @brief DLL exporting
#if defined(_WIN32)
    #if defined(COSMOS_EXPORT)
        #define COSMOS_API __declspec(dllexport)
    #else
        #define COSMOS_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #if defined(COSMOS_EXPORT)
        #define COSMOS_API __attribute__((visibility("default")))
    #else
        #define COSMOS_API
    #endif
#else
    #define COSMOS_API
#endif