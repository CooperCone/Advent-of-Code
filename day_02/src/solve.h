#pragma once

#include <windows.h>

typedef struct
{
    ULONGLONG solve1;
    ULONGLONG solve2;
} Result02;

#ifdef __cplusplus
extern "C"
{
#endif

Result02 solve02(LPCWSTR filename);

#ifdef __cplusplus
}
#endif

