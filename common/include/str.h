#pragma once

#include <windows.h>

#include "common.h"

typedef struct
{
    PBYTE str;
    SIZE_T size;
} String;

BOOL COMMON split(String *string, LPCSTR delimiter, String **out_strs,
    SIZE_T *out_num_strs);

String COMMON tokenize(String *string, LPCSTR delimiter);

BOOL COMMON is_digit(BYTE character);

BOOL COMMON is_symbol(BYTE character);

String COMMON string_prefix(String string, LPCSTR substr);

String COMMON string_find(String string, CHAR c);

BOOL COMMON string_cmp(String s1, String s2);

SIZE_T COMMON cstrlen(LPCSTR str);

SIZE_T COMMON wstrlen(LPCWSTR str);

BOOL COMMON consume(String *string, SIZE_T num_to_consume);

BOOL COMMON consume_if(String *string, LPCSTR prefix);

BOOL COMMON parse_uint(String *string, PULONGLONG out_number);

BOOL COMMON parse_int(String *string, PLONGLONG out_number);

BOOL COMMON copy_memory(void *dst, void *src, SIZE_T num_bytes);
