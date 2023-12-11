#pragma once

#include <windows.h>

typedef struct
{
    ULONGLONG solve1;
    ULONGLONG solve2;
} Result08;

#ifdef __cplusplus
extern "C"
{
#endif

Result08 solve08(LPCWSTR filename);

#ifdef __cplusplus
}
#endif

