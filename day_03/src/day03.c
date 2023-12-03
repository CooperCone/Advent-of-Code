#include "solve.h"

#include "str.h"
#include "io.h"

static BOOL find_adjacent_part(SIZE_T column, String top_line, String mid_line,
    String bot_line, PULONGLONG out_part_number);

static BOOL find_two_adjacent_parts(SIZE_T column, String top_line,
    String mid_line, String bot_line, PULONGLONG part_1, PULONGLONG part_2);

static BOOL find_part_number(String line, SIZE_T col,
    PULONGLONG out_part_number);

static BOOL find_part_number_with_neg(String line, SIZE_T col,
    PULONGLONG out_part_number);

static BOOL parse_uint_with_neg(String *string, PULONGLONG out_number);

Result03 solve03(LPCWSTR filename)
{
    Result03 result = {0};

    SIZE_T file_size = 0;

    PBYTE file_data = read_file(filename, &file_size);

    if (file_data == NULL)
    {
        goto COMPLETE;
    }

    String file_str =
    {
        .str = file_data,
        .size = file_size
    };

    String top_line = {0};
    String mid_line = tokenize(&file_str, "\r\n");
    String bot_line = tokenize(&file_str, "\r\n");

    if ((mid_line.size == 0) || (bot_line.size == 0))
    {
        goto CLEANUP_FILE;
    }

    while (mid_line.size > 0)
    {
        String parts = mid_line;

        while (parts.size > 0)
        {
            if (is_symbol(*parts.str) && (*parts.str != '.'))
            {
                SIZE_T column = mid_line.size - parts.size;

                ULONGLONG part_number = 0;

                while (find_adjacent_part(column, top_line, mid_line, bot_line,
                    &part_number))
                {
                    result.solve1 += part_number;
                }

                if (*parts.str == '*')
                {
                    ULONGLONG part_1 = 0;
                    ULONGLONG part_2 = 0;

                    if (find_two_adjacent_parts(column, top_line, mid_line,
                            bot_line, &part_1, &part_2))
                    {
                        result.solve2 += part_1 * part_2;
                    }
                }
            }

            if (!consume(&parts, 1))
            {
                goto CLEANUP_FILE;
            }
        }

        top_line = mid_line;
        mid_line = bot_line;
        bot_line = tokenize(&file_str, "\r\n");
    }

CLEANUP_FILE:
    HeapFree(GetProcessHeap(), 0, file_data);
    file_data = NULL;

COMPLETE:
    return result;
}

static BOOL find_adjacent_part(SIZE_T column, String top_line, String mid_line,
    String bot_line, PULONGLONG out_part_number)
{
    if ((column >= mid_line.size) || (mid_line.str == NULL) ||
        (mid_line.size == 0) || (out_part_number == NULL))
    {
        return FALSE;
    }

    SIZE_T left = column > 0 ? column - 1 : column;
    SIZE_T right = column < mid_line.size - 1 ? column + 1 : column;

    String lines[3] = { top_line, mid_line, bot_line };

    for (SIZE_T row = 0; row < 3; row++)
    {
        for (SIZE_T col = left; col <= right; col++)
        {
            if ((lines[row].size > 0) && (lines[row].str != NULL))
            {
                if (is_digit(lines[row].str[col]))
                {
                    if (find_part_number(lines[row], col, out_part_number))
                    {
                        return TRUE;
                    }
                }
            }
        }
    }

    return FALSE;
}

static BOOL find_two_adjacent_parts(SIZE_T column, String top_line,
    String mid_line, String bot_line, PULONGLONG part_1, PULONGLONG part_2)
{
    if ((column >= mid_line.size) || (mid_line.str == NULL) ||
        (mid_line.size == 0) || (part_1 == NULL) || ((part_2 == NULL)))
    {
        return FALSE;
    }

    SIZE_T left = column > 0 ? column - 1 : column;
    SIZE_T right = column < mid_line.size - 1 ? column + 1 : column;

    String lines[3] = { top_line, mid_line, bot_line };

    BOOL saw_dot_last = TRUE;

    SIZE_T num_parts = 0;
    ULONGLONG parts[2] = {0};

    BOOL result = TRUE;

    for (SIZE_T row = 0; row < 3; row++)
    {
        for (SIZE_T col = left; col <= right; col++)
        {
            BYTE c = lines[row].str[col] & 0x7F;

            if (saw_dot_last && is_digit(c))
            {
                if (num_parts == 2)
                {
                    result = FALSE;
                    goto COMPLETE;
                }

                if (!find_part_number_with_neg(lines[row], col,
                        parts + num_parts))
                {
                    result = FALSE;
                    goto COMPLETE;
                }

                num_parts++;
                saw_dot_last = FALSE;
            }
            else if (!saw_dot_last && !is_digit(c))
            {
                saw_dot_last = TRUE;
            }
        }

        saw_dot_last = TRUE;
    }

    if (num_parts != 2)
    {
        result = FALSE;
    }

    if (result)
    {
        *part_1 = parts[0];
        *part_2 = parts[1];
    }

COMPLETE:
    return result;
}

static BOOL find_part_number(String line, SIZE_T col,
    PULONGLONG out_part_number)
{
    if ((line.str == NULL) || (line.size == 0) || (col >= line.size) ||
        (out_part_number == NULL))
    {
        return FALSE;
    }

    while ((col > 0) && (is_digit(line.str[col - 1])))
    {
        col--;
    }

    String part =
    {
        .str = line.str + col,
        .size = line.size - col
    };

    if (!parse_uint(&part, out_part_number))
    {
        return FALSE;
    }

    for (SIZE_T i = col; i < line.size - part.size; i++)
    {
        line.str[i] |= 0x80;
    }

    return TRUE;
}

static BOOL find_part_number_with_neg(String line, SIZE_T col,
    PULONGLONG out_part_number)
{
    if ((line.str == NULL) || (line.size == 0) || (col >= line.size) ||
        (out_part_number == NULL))
    {
        return FALSE;
    }

    while ((col > 0) && (is_digit(line.str[col - 1] & 0x7F)))
    {
        col--;
    }

    String part =
    {
        .str = line.str + col,
        .size = line.size - col
    };

    if (!parse_uint_with_neg(&part, out_part_number))
    {
        return FALSE;
    }

    for (SIZE_T i = col; i < line.size - part.size; i++)
    {
        line.str[i] |= 0x80;
    }

    return TRUE;
}

static BOOL parse_uint_with_neg(String *string, PULONGLONG out_number)
{
    if ((string == NULL) || (string->str == NULL) || (string->size == 0) ||
        (out_number == NULL))
    {
        return FALSE;
    }

    if (!is_digit((*string->str) & 0x7F))
    {
        return FALSE;
    }

    ULONGLONG number = 0;

    while (string->size > 0)
    {
        if (!is_digit((*string->str) & 0x7F))
        {
            break;
        }

        number *= 10;
        number += (*string->str & 0x7F) - '0';

        if (!consume(string, 1))
        {
            break;
        }
    }

    *out_number = number;

    return TRUE;
}
