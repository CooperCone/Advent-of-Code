#include "common.h"

static BOOL compare_delimiter(PBYTE str, PBYTE end, PBYTE delimiter);

static SIZE_T wstrlen(LPCWSTR str);

PBYTE COMMON read_file(LPCWSTR filename, PSIZE_T out_size)
{
    if (filename == NULL)
    {
        return NULL;
    }

    PBYTE data = NULL;

    HANDLE file = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);

    if (file == INVALID_HANDLE_VALUE)
    {
        goto COMPLETE;
    }

    LARGE_INTEGER file_size = {0};

    if (!GetFileSizeEx(file, &file_size))
    {
        goto CLEANUP_FILE;
    }

    data = HeapAlloc(GetProcessHeap(), 0, file_size.QuadPart);

    if (data == NULL)
    {
        goto CLEANUP_FILE;
    }

    LONGLONG total_bytes_read = 0;

    while (total_bytes_read < file_size.QuadPart)
    {
        DWORD bytes_to_read = (DWORD)(file_size.QuadPart - total_bytes_read);

        DWORD bytes_read = 0;

        if (!ReadFile(file, data + total_bytes_read, bytes_to_read, &bytes_read,
            NULL))
        {
            HeapFree(GetProcessHeap(), 0, data);
            data = NULL;
            goto CLEANUP_FILE;
        }

        total_bytes_read += bytes_read;
    }

    *out_size = (SIZE_T)file_size.QuadPart;

CLEANUP_FILE:
    CloseHandle(file);

COMPLETE:
    return data;
}

PBYTE COMMON tokenize(PBYTE *location, PBYTE end, PBYTE delimiter,
    PSIZE_T out_size)
{
    if ((location == NULL) || (*location == NULL) || (end == NULL) ||
        (delimiter == NULL) || (out_size == NULL) || (*location > end))
    {
        return NULL;
    }

    PBYTE data = *location;
    PBYTE start = data;

    if (compare_delimiter(data, end, delimiter))
    {
        PBYTE tmp = delimiter;

        while (*tmp != '\0')
        {
            data++;
            start++;
            tmp++;
        }
    }

    while (data <= end)
    {
        if (compare_delimiter(data, end, delimiter))
        {
            break;
        }

        data++;
    }

    *location = data;
    *out_size = data - start;

    return start;
}

BOOL COMMON is_digit(BYTE character)
{
    return (character >= '0') && (character <= '9');
}

void COMMON print_solve_number(LPCWSTR header, ULONGLONG number)
{
    HANDLE stdout = GetStdHandle(STD_OUTPUT_HANDLE);

    if (stdout == INVALID_HANDLE_VALUE)
    {
        return;
    }

    if (!WriteConsoleW(stdout, header, (DWORD)wstrlen(header), NULL, NULL))
    {
        return;
    }

    if (!WriteConsoleW(stdout, L" ", 1, NULL, NULL))
    {
        return;
    }

    WCHAR buff[64] = {0};
    SIZE_T buff_size = sizeof buff / sizeof *buff;

    ULONGLONG remaining_num = number;

    SIZE_T num_length = 0;

    for (SIZE_T i = 0; i < buff_size; i++)
    {
        ULONGLONG remainder = remaining_num % 10;
        remaining_num = remaining_num / 10;

        buff[buff_size - i - 1] = L'0' + (WCHAR)remainder;

        if (remaining_num == 0)
        {
            num_length = i + 1;
            break;
        }
    }

    if (!WriteConsoleW(stdout, buff + buff_size - num_length, (DWORD)num_length,
            NULL, NULL))
    {
        return;
    }

    if (!WriteConsoleW(stdout, L"\n", 1, NULL, NULL))
    {
        return;
    }

    return;
}

BOOL COMMON has_substr(PBYTE str, SIZE_T size, LPCSTR substr)
{
    if ((str == NULL) || (size == 0) || (substr == NULL))
    {
        return FALSE;
    }

    BOOL result = FALSE;

    for (SIZE_T i = 0; i < size; i++)
    {
        if (substr[i] == '\0')
        {
            result = TRUE;
            break;
        }

        if (str[i] != substr[i])
        {
            break;
        }

        if ((i == size - 1) && (substr[i + 1] == '\0'))
        {
            result = TRUE;
            break;
        }
    }

    return result;
}

static BOOL compare_delimiter(PBYTE str, PBYTE end, PBYTE delimiter)
{
    if ((str == NULL) || (end == NULL) || (delimiter == NULL))
    {
        return FALSE;
    }

    BOOL is_valid = FALSE;

    for (;;)
    {
        if (*delimiter == '\0')
        {
            is_valid = TRUE;
            break;
        }

        if ((str > end) || (*str != *delimiter))
        {
            break;
        }

        str++;
        delimiter++;
    }

    return is_valid;
}

static SIZE_T wstrlen(LPCWSTR str)
{
    if (str == NULL)
    {
        return 0;
    }

    SIZE_T size = 0;

    while (*str != L'\0')
    {
        size++;
        str++;
    }

    return size;
}
