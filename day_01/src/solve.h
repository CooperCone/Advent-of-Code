#pragma once

#include <windows.h>

typedef struct
{
    ULONGLONG solve1;
    ULONGLONG solve2;
} Result01;

#ifdef __cplusplus
extern "C"
{
#endif

Result01 solve01(LPCWSTR filename);

#ifdef __cplusplus
}
#endif

