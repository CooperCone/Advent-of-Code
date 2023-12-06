#pragma once

#include <windows.h>

typedef struct
{
    ULONGLONG solve1;
    ULONGLONG solve2;
} Result06;

#ifdef __cplusplus
extern "C"
{
#endif

Result06 solve06(LPCWSTR filename);

#ifdef __cplusplus
}
#endif

