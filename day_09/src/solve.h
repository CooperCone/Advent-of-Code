#pragma once

#include <windows.h>

typedef struct
{
    ULONGLONG solve1;
    ULONGLONG solve2;
} Result09;

#ifdef __cplusplus
extern "C"
{
#endif

Result09 solve09(LPCWSTR filename);

#ifdef __cplusplus
}
#endif

