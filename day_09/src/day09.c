#include "solve.h"

#include "str.h"
#include "io.h"

static BOOL parse_line(String line, PLONGLONG *out_numbers,
    PSIZE_T out_num_numbers);

static BOOL calculate_predictions(PLONGLONG numbers, SIZE_T num_numbers,
    PLONGLONG out_left_prediction, PLONGLONG out_right_prediction);

Result09 solve09(LPCWSTR filename)
{
    Result09 result = {0};

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

        LONGLONG *numbers = NULL;
        SIZE_T num_numbers = 0;

        if (!parse_line(line, &numbers, &num_numbers))
        {
            goto CLEANUP_FILE;
        }

        LONGLONG left_prediction = 0;
        LONGLONG right_prediction = 0;

        if (!calculate_predictions(numbers, num_numbers, &left_prediction,
            &right_prediction))
        {
            HeapFree(GetProcessHeap(), 0, numbers);
            numbers = NULL;
            num_numbers = 0;

            goto CLEANUP_FILE;
        }

        result.solve1 += right_prediction;
        result.solve2 += left_prediction;
    }

CLEANUP_FILE:
    HeapFree(GetProcessHeap(), 0, file_data);
    file_data = NULL;

COMPLETE:
    return result;
}

static BOOL parse_line(String line, PLONGLONG *out_numbers,
    PSIZE_T out_num_numbers)
{
    if ((line.str == NULL) || (line.size == 0) || (out_numbers == NULL) ||
        (out_num_numbers == 0))
    {
        return FALSE;
    }

    SIZE_T num_numbers = 0;

    String cur = line;

    while (cur.size > 0)
    {
        num_numbers++;

        LONGLONG number = 0;

        if (!parse_int(&cur, &number))
        {
            goto ERROR_NO_CLEANUP;
        }

        consume_if(&cur, " ");
    }

    PLONGLONG numbers = HeapAlloc(GetProcessHeap(), 0,
        num_numbers * sizeof *numbers);

    if (numbers == NULL)
    {
        goto ERROR_NO_CLEANUP;
    }

    cur = line;

    for (SIZE_T i = 0; i < num_numbers; i++)
    {
        if (!parse_int(&cur, numbers + i))
        {
            goto ERROR_CLEANUP_NUMBERS;
        }

        consume_if(&cur, " ");
    }

    *out_numbers = numbers;
    *out_num_numbers = num_numbers;

    return TRUE;

ERROR_CLEANUP_NUMBERS:
    HeapFree(GetProcessHeap(), 0, numbers);
    numbers = NULL;
    num_numbers = 0;

ERROR_NO_CLEANUP:
    return FALSE;
}

static BOOL calculate_predictions(PLONGLONG numbers, SIZE_T num_numbers,
    PLONGLONG out_left_prediction, PLONGLONG out_right_prediction)
{
    if ((numbers == NULL) || (num_numbers <= 1) ||
        (out_left_prediction == NULL) || (out_right_prediction == NULL))
    {
        return FALSE;
    }

    LONGLONG left_prediction = 0;
    LONGLONG right_prediction = 0;

    BOOL all_zeros = TRUE;

    for (SIZE_T i = 0; i < num_numbers; i++)
    {
        if (numbers[i] != 0)
        {
            all_zeros = FALSE;
            break;
        }
    }

    if (!all_zeros)
    {
        LONGLONG *diffs = HeapAlloc(GetProcessHeap(), 0,
            (num_numbers - 1) * sizeof *diffs);

        if (diffs == NULL)
        {
            goto ERROR_NO_CLEANUP;
        }

        for (SIZE_T i = 0; i < num_numbers - 1; i++)
        {
            diffs[i] = numbers[i + 1] - numbers[i];
        }

        LONGLONG diff_left_prediction = 0;
        LONGLONG diff_right_prediction = 0;
            
        if (!calculate_predictions(diffs, num_numbers - 1,
            &diff_left_prediction, &diff_right_prediction))
        {
            HeapFree(GetProcessHeap(), 0, diffs);
            diffs = NULL;
            goto ERROR_NO_CLEANUP;
        }

        left_prediction = numbers[0] - diff_left_prediction;
        right_prediction = numbers[num_numbers - 1] + diff_right_prediction;

        HeapFree(GetProcessHeap(), 0, diffs);
        diffs = NULL;
    }

    *out_left_prediction = left_prediction;
    *out_right_prediction = right_prediction;
    return TRUE;

ERROR_NO_CLEANUP:
    return FALSE;
}

