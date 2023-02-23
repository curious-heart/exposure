// IRayCommon.h
//

#pragma once


#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define SDK_CAPI_EXPORT extern "C" __declspec(dllexport)
#define SDK_CAPI_IMPORT extern "C" __declspec(dllimport)

#elif defined(linux) || defined(__linux__) || defined(__linux)
#define SDK_CAPI_IMPORT extern "C"
#define SDK_CAPI_EXPORT extern "C" 

#endif




