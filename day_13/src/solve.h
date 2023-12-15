#pragma once

#include <windows.h>

typedef struct
{
    ULONGLONG solve1;
    ULONGLONG solve2;
} Result13;

#ifdef __cplusplus
extern "C"
{
#endif

Result13 solve13(LPCWSTR filename);

#ifdef __cplusplus
}
#endif

