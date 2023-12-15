#include "solve.h"

#include "str.h"
#include "io.h"

typedef struct
{
    SIZE_T num_springs;
    SIZE_T num_numbers;
    SIZE_T max_number;
    LONGLONG *data;
} DPTable;

static ULONGLONG calculate_num_combinations(DPTable *table, String springs,
    String numbers, SIZE_T spring_pos, SIZE_T num_pos, SIZE_T num_block_size);

static BOOL create_unfolded_string(String original, String *out_string);

static BOOL extract_numbers(String line, String *out_numbers);

static BOOL create_dp_table(String line, String numbers, DPTable *out_dp_table);

Result12 solve12(LPCWSTR filename)
{
    Result12 result = {0};

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

        String numbers = {0};

        if (!extract_numbers(line, &numbers))
        {
            goto CLEANUP_FILE_DATA;
        }

        DPTable table = {0};

        if (!create_dp_table(line, numbers, &table))
        {
            goto CLEANUP_FILE_DATA;
        }

        result.solve1 += calculate_num_combinations(&table, line, numbers,
            0, 0, 0);

        HeapFree(GetProcessHeap(), 0, table.data);
        table = (DPTable){0};

        String larger_line = {0};

        if (!create_unfolded_string(line, &larger_line))
        {
            goto CLEANUP_FILE_DATA;
        }

        if (!extract_numbers(larger_line, &numbers))
        {
            goto CLEANUP_FILE_DATA;
        }

        if (!create_dp_table(larger_line, numbers, &table))
        {
            goto CLEANUP_FILE_DATA;
        }

        result.solve2 += calculate_num_combinations(&table, larger_line,
            numbers, 0, 0, 0);

        HeapFree(GetProcessHeap(), 0, table.data);
        table = (DPTable){0};

        HeapFree(GetProcessHeap(), 0, larger_line.str);
        larger_line = (String){0};
    }

CLEANUP_FILE_DATA:
    HeapFree(GetProcessHeap(), 0, file_data);
    file_data = NULL;

COMPLETE:
    return result;
}

static ULONGLONG calculate_num_combinations(DPTable *table, String springs,
    String numbers, SIZE_T spring_pos, SIZE_T num_pos, SIZE_T num_block_size)
{
    if ((springs.str == NULL) || (springs.size == 0) || (numbers.str == NULL) ||
        (table == NULL) || (table->data == NULL) || (table->max_number == 0) ||
        (table->num_numbers == 0) || (table->num_springs == 0))
    {
        return 0;
    }

    SIZE_T table_idx = spring_pos * table->num_numbers * table->max_number +
        num_pos * table->max_number +
        num_block_size;

    ULONGLONG result = 0;

    ULONGLONG number = 0;
    String consumed_numbers = numbers;

    if (numbers.size > 0)
    {
        if (!parse_uint(&consumed_numbers, &number))
        {
            goto COMPLETE;
        }

        consume_if(&consumed_numbers, ",");
    }

    if (table->data[table_idx] != -1)
    {
        result = table->data[table_idx];
    }
    // end of springs
    else if (springs.str[spring_pos] == ' ')
    {
        // // no numbers left
        if (numbers.size == 0)
        {
            result = 1;
        }
        // one number left and we're at the end of the block
        else if ((consumed_numbers.size == 0) && (number == num_block_size))
        {
            result = 1;
        }
        // has numbers left
        else
        {
            result = 0;
        }
    }
    // spring = .
    else if (springs.str[spring_pos] == '.')
    {
        // no numbers left
        if (numbers.size == 0)
        {
            result = calculate_num_combinations(table, springs, numbers,
                spring_pos + 1, num_pos, num_block_size);
        }
        // block size == 0
        else if (num_block_size == 0)
        {
            result = calculate_num_combinations(table, springs, numbers,
                spring_pos + 1, num_pos, num_block_size);
        }
        // block size < number
        else if (num_block_size < number)
        {
            result = 0;
        }
        // block size == number
        else if (num_block_size == number)
        {
            result = calculate_num_combinations(table, springs, consumed_numbers,
                spring_pos + 1, num_pos + 1, 0);
        }
        else
        {
            goto COMPLETE;
        }
    }
    // spring = #
    else if (springs.str[spring_pos] == '#')
    {
        // no number left
        if (numbers.size == 0)
        {
            result = 0;
        }
        // block size == number
        else if (num_block_size == number)
        {
            result = 0;
        }
        else
        {
            result = calculate_num_combinations(table, springs, numbers,
                spring_pos + 1, num_pos, num_block_size + 1);
        }
    }
    // spring = ?
    else if (springs.str[spring_pos] == '?')
    {
        // no number left
        if (numbers.size == 0)
        {
            // can only check if it's a .
            result = calculate_num_combinations(table, springs, numbers,
                spring_pos + 1, num_pos, num_block_size);
        }
        // block size == number
        else if (num_block_size == number)
        {
            // can only check if it's a .
            result = calculate_num_combinations(table, springs,
                consumed_numbers, spring_pos + 1, num_pos + 1, 0);
        }
        else if (num_block_size == 0)
        {
            // check if . or #
            result = calculate_num_combinations(table, springs, numbers,
                    spring_pos + 1, num_pos, num_block_size) +
                calculate_num_combinations(table, springs, numbers,
                spring_pos + 1, num_pos, num_block_size + 1);
        }
        else
        {
            result = calculate_num_combinations(table, springs, numbers,
                spring_pos + 1, num_pos, num_block_size + 1);
        }
    }
    else
    {
        goto COMPLETE;
    }

    table->data[table_idx] = result;

COMPLETE:
    return result;
}

static BOOL create_unfolded_string(String original, String *out_string)
{
    if ((original.str == NULL) || (original.size == 0) || (out_string == NULL))
    {
        return FALSE;
    }

    BOOL result = FALSE;

    SIZE_T num_springs = 0;
    String copy = original;

    while (copy.str[0] != ' ')
    {
        consume(&copy, 1);
        num_springs++;
    }

    consume(&copy, 1);

    SIZE_T num_numbers = copy.size;

    SIZE_T needed_space =
        (num_springs + 1) * 5 - 1 + 1 + (num_numbers + 1) * 5 - 1;

    PBYTE str_data = HeapAlloc(GetProcessHeap(), 0, needed_space);

    if (str_data == NULL)
    {
        goto ERROR_NO_CLEANUP;
    }

    for (SIZE_T i = 0; i < 5; i++)
    {
        if (!copy_memory(str_data + (num_springs + 1) * i, original.str,
                num_springs))
        {
            goto ERROR_CLEANUP_DATA;
        }

        if (i != 4)
        {
            str_data[(num_springs + 1) * i + num_springs] = '?';
        }
    }

    str_data[(num_springs + 1) * 5 - 1] = ' ';

    SIZE_T base = (num_springs + 1) * 5 + 1 - 1;

    for (SIZE_T i = 0; i < 5; i++)
    {
        if (!copy_memory(str_data + base + (num_numbers + 1) * i,
                original.str + num_springs + 1, num_numbers))
        {
            goto ERROR_CLEANUP_DATA;
        }

        if (i != 4)
        {
            str_data[base + (num_numbers + 1) * i + num_numbers] = ',';
        }
    }

    *out_string = (String){ .str = str_data, .size = needed_space };

    return TRUE;

ERROR_CLEANUP_DATA:
    HeapFree(GetProcessHeap(), 0, str_data);
    str_data = NULL;

ERROR_NO_CLEANUP:
    return FALSE;
}

static BOOL extract_numbers(String line, String *out_numbers)
{
    if ((line.str == NULL) || (line.size == 0) || (out_numbers == NULL))
    {
        return FALSE;
    }

    BOOL result = FALSE;

    String numbers = line;

    while (numbers.str[0] != ' ')
    {
        if (!consume(&numbers, 1))
        {
            goto COMPLETE;
        }
    }

    if (!consume(&numbers, 1))
    {
        goto COMPLETE;
    }

    *out_numbers = numbers;

    result = TRUE;

COMPLETE:
    return result;
}

static BOOL create_dp_table(String line, String numbers, DPTable *out_dp_table)
{
    if ((line.str == NULL) || (line.size == 0) || (numbers.str == NULL) ||
        (numbers.size == 0) || (out_dp_table == NULL))
    {
        return FALSE;
    }

    BOOL result = FALSE;

    SIZE_T num_springs = 0;


    while (line.str[0] != ' ')
    {
        if (!consume(&line, 1))
        {
            goto COMPLETE;
        }
        num_springs++;
    }

    num_springs++;

    SIZE_T num_numbers = 0;
    ULONGLONG max_number = 0;

    while (numbers.size > 0)
    {
        ULONGLONG number = 0;

        if (!parse_uint(&numbers, &number))
        {
            goto COMPLETE;
        }

        consume_if(&numbers, ",");

        num_numbers++;
        max_number = max(number, max_number);
    }

    num_numbers++;
    max_number++;

    ULONGLONG *table = HeapAlloc(GetProcessHeap(), 0,
        num_springs * num_numbers * max_number * sizeof *table);

    if (table == NULL)
    {
        goto COMPLETE;
    }

    for (SIZE_T i = 0; i < num_springs * num_numbers * max_number; i++)
    {
        table[i] = -1;
    }

    *out_dp_table = (DPTable)
    {
        .num_springs = num_springs,
        .num_numbers = num_numbers,
        .max_number = max_number,
        .data = table,
    };

    result = TRUE;

COMPLETE:
    return result;
}

