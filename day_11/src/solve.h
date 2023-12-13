#pragma once

#include <windows.h>

typedef struct
{
    ULONGLONG solve1;
    ULONGLONG solve2;
} Result11;

#ifdef __cplusplus
extern "C"
{
#endif

Result11 solve11(LPCWSTR filename);

#ifdef __cplusplus
}
#endif

