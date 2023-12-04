#pragma once

#include <windows.h>

typedef struct
{
    ULONGLONG solve1;
    ULONGLONG solve2;
} Result04;

#ifdef __cplusplus
extern "C"
{
#endif

Result04 solve04(LPCWSTR filename);

#ifdef __cplusplus
}
#endif

