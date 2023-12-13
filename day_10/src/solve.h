#pragma once

#include <windows.h>

typedef struct
{
    ULONGLONG solve1;
    ULONGLONG solve2;
} Result10;

#ifdef __cplusplus
extern "C"
{
#endif

Result10 solve10(LPCWSTR filename);

#ifdef __cplusplus
}
#endif

