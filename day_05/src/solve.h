#pragma once

#include <windows.h>

typedef struct
{
    ULONGLONG solve1;
    ULONGLONG solve2;
} Result05;

#ifdef __cplusplus
extern "C"
{
#endif

Result05 solve05(LPCWSTR filename);

#ifdef __cplusplus
}
#endif

