#pragma once

#include <windows.h>

typedef struct
{
    ULONGLONG solve1;
    ULONGLONG solve2;
} Result03;

#ifdef __cplusplus
extern "C"
{
#endif

Result03 solve03(LPCWSTR filename);

#ifdef __cplusplus
}
#endif

