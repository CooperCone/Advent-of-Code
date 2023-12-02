#pragma once

#include <windows.h>

#define COMMON __declspec(dllexport)

PBYTE COMMON read_file(LPCWSTR filename, PSIZE_T out_size);

PBYTE COMMON tokenize(PBYTE *location, PBYTE end, PBYTE delimiter,
    PSIZE_T out_size);

BOOL COMMON is_digit(BYTE character);

void COMMON print_solve_number(LPCWSTR header, ULONGLONG number);

BOOL COMMON has_substr(PBYTE str, SIZE_T size, LPCSTR substr);
