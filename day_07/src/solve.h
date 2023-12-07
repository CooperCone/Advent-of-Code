#pragma once

#include <windows.h>

typedef struct
{
    ULONGLONG solve1;
    ULONGLONG solve2;
} Result07;

#ifdef __cplusplus
extern "C"
{
#endif

Result07 solve07(LPCWSTR filename);

#ifdef __cplusplus
}
#endif

