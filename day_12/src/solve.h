#pragma once

#include <windows.h>

typedef struct
{
    ULONGLONG solve1;
    ULONGLONG solve2;
} Result12;

#ifdef __cplusplus
extern "C"
{
#endif

Result12 solve12(LPCWSTR filename);

#ifdef __cplusplus
}
#endif

