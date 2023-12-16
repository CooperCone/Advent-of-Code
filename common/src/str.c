#include "str.h"

static BOOL compare_delimiter(String string, LPCSTR delimiter);

BOOL COMMON split(String *string, LPCSTR delimiter, String **out_strs,
    SIZE_T *out_num_strs)
{
    if ((string == NULL) || (string->str == NULL) || (string->size == 0) ||
        (delimiter == NULL) || (out_strs == NULL) || (out_num_strs == NULL))
    {
        return FALSE;
    }

    BOOL result = FALSE;

    SIZE_T num_lines = 0;
    SIZE_T max_lines = 8;
    String *lines = HeapAlloc(GetProcessHeap(), 0, max_lines * sizeof *lines);

    if (lines == NULL)
    {
        goto ERROR_NO_CLEANUP;
    }

    String str = *string;

    for (;;)
    {
        String line = tokenize(&str, "\r\n");

        if (line.size == 0)
        {
            break;
        }

        num_lines++;

        if (num_lines > max_lines)
        {
            max_lines *= 2;

            String *new_lines = HeapReAlloc(GetProcessHeap(), 0, lines,
                max_lines * sizeof *lines);

            if (new_lines == NULL)
            {
                goto ERROR_CLEANUP_LINES;
            }

            lines = new_lines;
        }

        lines[num_lines - 1] = line;
    }

    *out_strs = lines;
    *out_num_strs = num_lines;

    return TRUE;

ERROR_CLEANUP_LINES:
    HeapFree(GetProcessHeap(), 0, lines);
    lines = NULL;

ERROR_NO_CLEANUP:
    return FALSE;
}

String COMMON tokenize(String *string, LPCSTR delimiter)
{
    if ((string == NULL) || (string->size == 0) || (string->str == NULL) ||
        (delimiter == NULL))
    {
        return (String){0};
    }

    SIZE_T delim_len = cstrlen(delimiter);

    if (compare_delimiter(*string, delimiter))
    {
        string->str += delim_len;
        string->size -= delim_len;
    }

    if (string->size == 0)
    {
        return (String){0};
    }

    String result =
    {
        .str = string->str,
        .size = 0
    };

    while (string->size > 0)
    {
        if (compare_delimiter(*string, delimiter))
        {
            break;
        }

        string->str++;
        string->size--;

        result.size++;
    }

    return result;
}

BOOL COMMON is_digit(BYTE character)
{
    return (character >= '0') && (character <= '9');
}

BOOL COMMON is_symbol(BYTE character)
{
    return ((character >= '!') && (character <= '/')) ||
        ((character >= ':') && (character <= '@')) ||
        ((character >= '[') && (character <= '`')) ||
        ((character >= '{') && (character <= '~'));
}

String COMMON string_prefix(String string, LPCSTR substr)
{
    if ((string.str == NULL) || (string.size == 0) || (substr == NULL))
    {
        return (String){0};
    }

    String result = {0};

    for (SIZE_T i = 0; i < string.size; i++)
    {
        if (substr[i] == '\0')
        {
            result = string;
            result.size = i + 1;
            break;
        }

        if (string.str[i] != substr[i])
        {
            break;
        }

        if ((i == string.size - 1) && (substr[i + 1] == '\0'))
        {
            result = string;
            result.size = i + 1;
            break;
        }
    }

    return result;
}

String COMMON string_find(String string, CHAR c)
{
    if ((string.size == 0) || (string.str == NULL))
    {
        return (String){0};
    }

    String result = {0};

    while ((string.size > 0) && (*string.str != c))
    {
        if (!consume(&string, 1))
        {
            goto COMPLETE;
        }
    }

    if (string.size > 0)
    {
        result = string;
    }

COMPLETE:
    return result;
}

BOOL COMMON string_cmp(String s1, String s2)
{
    if ((s1.size != s2.size) || (s1.str == NULL) || (s2.str == NULL))
    {
        return FALSE;
    }

    BOOL are_same = TRUE;

    for (SIZE_T i = 0; i < s1.size; i++)
    {
        if (s1.str[i] != s2.str[i])
        {
            are_same = FALSE;
        }
    }

    return are_same;
}

SIZE_T COMMON cstrlen(LPCSTR str)
{
    if (str == NULL)
    {
        return 0;
    }

    SIZE_T size = 0;

    while (*str != '\0')
    {
        size++;
        str++;
    }

    return size;
}

SIZE_T COMMON wstrlen(LPCWSTR str)
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

BOOL COMMON consume(String *string, SIZE_T num_to_consume)
{
    if ((string == NULL) || (string->size == 0) || (string->str == NULL) ||
        (string->size < num_to_consume))
    {
        return FALSE;
    }

    string->str += num_to_consume;
    string->size -= num_to_consume;

    return TRUE;
}

BOOL COMMON consume_if(String *string, LPCSTR prefix)
{
    if ((string == NULL) || (prefix == NULL))
    {
        return FALSE;
    }

    BOOL result = FALSE;

    if (string_prefix(*string, prefix).str != NULL)
    {
        result = consume(string, cstrlen(prefix));
    }

    return result;
}

BOOL COMMON parse_uint(String *string, PULONGLONG out_number)
{
    if ((string == NULL) || (string->str == NULL) || (string->size == 0) ||
        (out_number == NULL))
    {
        return FALSE;
    }

    if (!is_digit(*string->str))
    {
        return FALSE;
    }

    ULONGLONG number = 0;

    while (string->size > 0)
    {
        if (!is_digit(*string->str))
        {
            break;
        }

        number *= 10;
        number += *string->str - '0';

        if (!consume(string, 1))
        {
            break;
        }
    }

    *out_number = number;

    return TRUE;
}

BOOL COMMON parse_int(String *string, PLONGLONG out_number)
{
    if ((string == NULL) || (string->str == NULL) || (string->size == 0) ||
        (out_number == NULL))
    {
        return FALSE;
    }

    BOOL result = FALSE;

    BOOL is_neg = FALSE;

    if (string->str[0] == '-')
    {
        is_neg = TRUE;
        consume(string, 1);
    }

    ULONGLONG abs_value = 0;

    if (!parse_uint(string, &abs_value))
    {
        goto COMPLETE;
    }

    if (abs_value > LLONG_MAX)
    {
        goto COMPLETE;
    }

    LONGLONG number = (LONGLONG)abs_value;

    if (is_neg)
    {
        number *= -1;
    }

    *out_number = number;

    result = TRUE;

COMPLETE:
    return result;
}

BOOL COMMON copy_memory(void *dst, void *src, SIZE_T num_bytes)
{
    if ((dst == NULL) || (src == NULL) || (num_bytes == 0))
    {
        return FALSE;
    }

    PBYTE dst_bytes = dst;
    PBYTE src_bytes = src;

    for (SIZE_T i = 0; i < num_bytes; i++)
    {
        dst_bytes[i] = src_bytes[i];
    }

    return TRUE;
}

static BOOL compare_delimiter(String string, LPCSTR delimiter)
{
    if ((string.str == NULL) || (string.size == 0) || (delimiter == NULL))
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

        if ((string.size == 0) || (*(string.str) != *delimiter))
        {
            break;
        }

        string.str++;
        string.size--;
        delimiter++;
    }

    return is_valid;
}
