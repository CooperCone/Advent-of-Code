#pragma once

#include <windows.h>

#include "common.h"

typedef struct
{
    PBYTE str;
    SIZE_T size;
} String;

String COMMON tokenize(String *string, LPCSTR delimiter);

BOOL COMMON is_digit(BYTE character);

String COMMON string_prefix(String string, LPCSTR substr);

SIZE_T COMMON cstrlen(LPCSTR str);

SIZE_T COMMON wstrlen(LPCWSTR str);

BOOL COMMON consume(String *string, SIZE_T num_to_consume);

BOOL COMMON consume_if(String *string, LPCSTR prefix);

BOOL COMMON parse_uint(String *string, PULONGLONG out_number);
