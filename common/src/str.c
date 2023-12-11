#include "str.h"

static BOOL compare_delimiter(String string, LPCSTR delimiter);

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
