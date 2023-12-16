#pragma once

#include <windows.h>

typedef struct
{
    ULONGLONG solve1;
    ULONGLONG solve2;
} Result14;

#ifdef __cplusplus
extern "C"
{
#endif

Result14 solve14(LPCWSTR filename);

#ifdef __cplusplus
}
#endif

