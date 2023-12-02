#include "solve.h"

#include "str.h"
#include "io.h"

typedef struct
{
    LONG digit1;
    LONG digit2;
} Digits;

static PBYTE digit_names[] =
{
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine",
};

static void get_puzzle1_digit(LONG *digit, String data);
static void get_puzzle2_digit(LONG *digit, String data);

static LONG match_str(String data);

Result01 solve01(LPCWSTR filename)
{
    Result01 result = {0};

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

    for (;;)
    {
        String line = tokenize(&file_str, "\r\n");

        if (line.size == 0)
        {
            break;
        }

        Digits solve1 = { -1, -1 };
        Digits solve2 = { -1, -1 };

        String left = line;
        String right =
        {
            .str = line.str + line.size - 1,
            .size = 1,
        };

        for (SIZE_T i = 0; i < line.size; i++)
        {
            get_puzzle1_digit(&solve1.digit1, left);
            get_puzzle1_digit(&solve1.digit2, right);

            get_puzzle2_digit(&solve2.digit1, left);
            get_puzzle2_digit(&solve2.digit2, right);

            left.str++;
            left.size--;

            right.str--;
            right.size++;
        }

        result.solve1 += solve1.digit1 * 10 + solve1.digit2;
        result.solve2 += solve2.digit1 * 10 + solve2.digit2;
    }

    HeapFree(GetProcessHeap(), 0, file_data);
    file_data = NULL;

COMPLETE:
    return result;
}

static void get_puzzle1_digit(LONG *digit, String data)
{
    if ((digit == NULL) || (data.str == NULL) || (data.size == 0))
    {
        return;
    }

    if ((*digit == -1) && is_digit(*data.str))
    {
        *digit = *data.str - '0';
    }
}

static void get_puzzle2_digit(LONG *digit, String data)
{
    if ((digit == NULL) || (data.str == NULL) || (data.size == 0))
    {
        return;
    }

    if (*digit == -1)
    {
        if (is_digit(*data.str))
        {
            *digit = *data.str - '0';
        }
        else
        {
            *digit = match_str(data);
        }
    }
}

static LONG match_str(String data)
{
    if (data.size < 3)
    {
        return -1;
    }

    LONG value = -1;

    for (SIZE_T i = 0; i < sizeof digit_names / sizeof *digit_names; i++)
    {
        if (string_prefix(data, digit_names[i]).str != NULL)
        {
            value = (LONG)i + 1;
        }
    }

    return value;
}
