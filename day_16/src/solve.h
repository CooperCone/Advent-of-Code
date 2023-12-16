#pragma once

#include <windows.h>

typedef struct
{
    ULONGLONG solve1;
    ULONGLONG solve2;
} Result16;

#ifdef __cplusplus
extern "C"
{
#endif

Result16 solve16(LPCWSTR filename);

#ifdef __cplusplus
}
#endif

