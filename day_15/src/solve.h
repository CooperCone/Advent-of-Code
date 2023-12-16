#pragma once

#include <windows.h>

typedef struct
{
    ULONGLONG solve1;
    ULONGLONG solve2;
} Result15;

#ifdef __cplusplus
extern "C"
{
#endif

Result15 solve15(LPCWSTR filename);

#ifdef __cplusplus
}
#endif

