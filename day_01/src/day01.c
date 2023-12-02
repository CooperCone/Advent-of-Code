#include <windows.h>

#include "solve.h"
#include "common.h"

typedef struct
{
    LONG digit1;
    LONG digit2;
} Digits;

static LPCWSTR example1 = L"res\\example1.txt";
static LPCWSTR example2 = L"res\\example2.txt";
static LPCWSTR input = L"res\\input.txt";

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

static void get_puzzle1_digit(LONG *digit, PBYTE data, SIZE_T data_len);
static void get_puzzle2_digit(LONG *digit, PBYTE data, SIZE_T data_len);

static LONG match_str(PBYTE data, SIZE_T data_len);

int mainCRTStartup()
{
    Result01 result = solve01(input);

    print_solve_number(L"Solve 1:", result.solve1);
    print_solve_number(L"Solve 2:", result.solve2);

    return 0;
}

Result01 solve01(LPCWSTR filename)
{
    Result01 result = {0};

    SIZE_T file_size = 0;

    PBYTE file_data = read_file(filename, &file_size);

    if (file_data == NULL)
    {
        goto COMPLETE;
    }

    PBYTE end = file_data + file_size - 1;
    PBYTE file_location = file_data;

    for (;;)
    {
        SIZE_T line_length = 0;

        PBYTE line = tokenize(&file_location, end, "\r\n", &line_length);

        if (line == NULL)
        {
            break;
        }

        Digits solve1 = { -1, -1 };
        Digits solve2 = { -1, -1 };

        for (SIZE_T i = 0; i < line_length; i++)
        {
            PCHAR left = line + i;
            PCHAR right = line + line_length - i - 1;

            SIZE_T left_len = line_length;
            SIZE_T right_len = i + 1;

            get_puzzle1_digit(&solve1.digit1, left, left_len);
            get_puzzle1_digit(&solve1.digit2, right, right_len);

            get_puzzle2_digit(&solve2.digit1, left, left_len);
            get_puzzle2_digit(&solve2.digit2, right, right_len);
        }

        result.solve1 += solve1.digit1 * 10 + solve1.digit2;
        result.solve2 += solve2.digit1 * 10 + solve2.digit2;
    }

    HeapFree(GetProcessHeap(), 0, file_data);
    file_data = NULL;

COMPLETE:
    return result;
}

static void get_puzzle1_digit(LONG *digit, PBYTE data, SIZE_T data_len)
{
    if ((digit == NULL) || (data == NULL) || (data_len == 0))
    {
        return;
    }

    if ((*digit == -1) && is_digit(*data))
    {
        *digit = *data - '0';
    }
}

static void get_puzzle2_digit(LONG *digit, PBYTE data, SIZE_T data_len)
{
    if ((digit == NULL) || (data == NULL) || (data_len == 0))
    {
        return;
    }

    if (*digit == -1)
    {
        if (is_digit(*data))
        {
            *digit = *data - '0';
        }
        else
        {
            *digit = match_str(data, data_len);
        }
    }
}

static LONG match_str(PBYTE data, SIZE_T data_len)
{
    if (data_len < 3)
    {
        return -1;
    }

    LONG value = -1;

    for (SIZE_T i = 0; i < sizeof digit_names / sizeof *digit_names; i++)
    {
        if (has_substr(data, data_len, digit_names[i]))
        {
            value = (LONG)i + 1;
        }
    }

    return value;
}
