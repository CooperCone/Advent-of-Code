#include "solve.h"

#include "str.h"
#include "io.h"

static BOOL find_block(String *file, String **out_lines, SIZE_T *out_num_lines);

static BOOL find_vertical_reflection_with_errors(String *block, SIZE_T width,
    SIZE_T height, SIZE_T num_errors, SIZE_T *out_num_left);

static SSIZE_T get_num_column_errors(String *block, SIZE_T width, SIZE_T height,
    SIZE_T left_col, SIZE_T right_col);

static BOOL find_horizontal_reflection_with_errors(String *block, SIZE_T width,
    SIZE_T height, SIZE_T num_errors, SIZE_T *out_num_above);

static SSIZE_T get_num_row_errors(String *block, SIZE_T width, SIZE_T height,
    SIZE_T top_row, SIZE_T bottom_row);

Result13 solve13(LPCWSTR filename)
{
    Result13 result = {0};

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

    SIZE_T num_horizontal_reflections = 0;
    SIZE_T num_vertical_reflections = 0;

    SIZE_T num_horizontal_reflections_2 = 0;
    SIZE_T num_vertical_reflections_2 = 0;

    SIZE_T num_blocks = 0;

    while (file_str.size > 0)
    {
        SIZE_T height = 0;
        String *block = NULL;

        if (!find_block(&file_str, &block, &height))
        {
            goto CLEANUP_FILE_DATA;
        }

        SIZE_T width = block[0].size;

        SIZE_T cols_or_rows = 0;

        if (find_vertical_reflection_with_errors(block, width, height, 0,
                &cols_or_rows))
        {
            num_vertical_reflections += cols_or_rows;
        }
        else if (find_horizontal_reflection_with_errors(block, width, height, 0,
                &cols_or_rows))
        {
            num_horizontal_reflections += cols_or_rows;
        }
        else
        {
            OutputDebugStringW(L"Shouldn't be here\n");
        }

        if (find_vertical_reflection_with_errors(block, width, height, 1,
                &cols_or_rows))
        {
            num_vertical_reflections_2 += cols_or_rows;
        }
        else if (find_horizontal_reflection_with_errors(block, width, height, 1,
                &cols_or_rows))
        {
            num_horizontal_reflections_2 += cols_or_rows;
        }
        else
        {
            OutputDebugStringW(L"Shouldn't be here\n");
        }

        HeapFree(GetProcessHeap(), 0, block);
        block = NULL;

        num_blocks++;
    }

    result.solve1 = num_vertical_reflections + num_horizontal_reflections * 100;
    result.solve2 = num_vertical_reflections_2 + num_horizontal_reflections_2 * 100;

CLEANUP_FILE_DATA:
    HeapFree(GetProcessHeap(), 0, file_data);
    file_data = NULL;

COMPLETE:
    return result;
}

static BOOL find_block(String *file, String **out_lines, SIZE_T *out_num_lines)
{
    if ((file == NULL) || (file->size == 0) || (file->str == NULL) ||
        (out_lines == NULL) || (out_num_lines == NULL))
    {
        return FALSE;
    }

    BOOL result = FALSE;

    SIZE_T num_lines = 0;
    String file_copy = *file;

    for (;;)
    {
        String line = tokenize(&file_copy, "\r\n");

        if ((line.str == NULL) || (line.size == 0))
        {
            break;
        }

        num_lines++;
    }

    String *lines = HeapAlloc(GetProcessHeap(), 0, num_lines * sizeof *lines);

    if (lines == NULL)
    {
        goto COMPLETE;
    }

    for (SIZE_T i = 0; i < num_lines; i++)
    {
        String line = tokenize(file, "\r\n");

        if ((line.str == NULL) || (line.size == 0))
        {
            break;
        }

        lines[i] = line;
    }

    consume_if(file, "\r\n");

    *out_lines = lines;
    *out_num_lines = num_lines;

    result = TRUE;

COMPLETE:
    return result;
}

static BOOL find_vertical_reflection_with_errors(String *block, SIZE_T width,
    SIZE_T height, SIZE_T num_errors, SIZE_T *out_num_left)
{
    if ((block == NULL) || (width == 0) || (height == 0) ||
        (out_num_left == NULL))
    {
        return FALSE;
    }

    LONGLONG reflection_col = -1;

    for (SIZE_T i = 0; i < width - 1; i++)
    {
        SIZE_T num_left = i + 1;
        SIZE_T num_right = width - i - 1;

        SIZE_T num_to_check = min(num_left, num_right);

        SIZE_T num_reflection_errors = 0;

        for (SIZE_T j = 0; j < num_to_check; j++)
        {
            SIZE_T left_col = i - j;
            SIZE_T right_col = i + j + 1;

            SSIZE_T found_errors = get_num_column_errors(block, width, height,
                left_col, right_col);

            if (found_errors == -1)
            {
                goto COMPLETE;
            }

            num_reflection_errors += found_errors;

            if (num_reflection_errors > (SIZE_T)num_errors)
            {
                break;
            }
        }

        if (num_reflection_errors == num_errors)
        {
            reflection_col = num_left;
            break;
        }
    }

    if (reflection_col != -1)
    {
        *out_num_left = (SIZE_T)reflection_col;
    }

COMPLETE:
    return reflection_col != -1;
}

static SSIZE_T get_num_column_errors(String *block, SIZE_T width, SIZE_T height,
    SIZE_T left_col, SIZE_T right_col)
{
    if ((block == NULL) || (width == 0) || (height == 0))
    {
        return -1;
    }

    SSIZE_T num_errors = 0;

    for (SIZE_T y = 0; y < height; y++)
    {
        if (block[y].str[left_col] != block[y].str[right_col])
        {
            num_errors++;
        }
    }

    return num_errors;
}

static BOOL find_horizontal_reflection_with_errors(String *block, SIZE_T width,
    SIZE_T height, SIZE_T num_errors, SIZE_T *out_num_above)
{
    if ((block == NULL) || (width == 0) || (height == 0) ||
        (out_num_above == NULL))
    {
        return FALSE;
    }

    LONGLONG reflection_row = -1;

    for (SIZE_T i = 0; i < height - 1; i++)
    {
        SIZE_T num_above = i + 1;
        SIZE_T num_below = height - i - 1;

        SIZE_T num_to_check = min(num_above, num_below);

        SIZE_T num_reflection_errors = 0;

        for (SIZE_T j = 0; j < num_to_check; j++)
        {
            SIZE_T top_row = i - j;
            SIZE_T bottom_row = i + j + 1;

            SSIZE_T found_errors = get_num_row_errors(block, width, height,
                top_row, bottom_row);

            if (found_errors == -1)
            {
                goto COMPLETE;
            }

            num_reflection_errors += found_errors;

            if (num_reflection_errors > (SIZE_T)num_errors)
            {
                break;
            }
        }

        if (num_reflection_errors == num_errors)
        {
            reflection_row = num_above;
            break;
        }
    }

    if (reflection_row != -1)
    {
        *out_num_above = (SIZE_T)reflection_row;
    }

COMPLETE:
    return reflection_row != -1;
}

static SSIZE_T get_num_row_errors(String *block, SIZE_T width, SIZE_T height,
    SIZE_T top_row, SIZE_T bottom_row)
{
    if ((block == NULL) || (width == 0) || (height == 0))
    {
        return -1;
    }

    SSIZE_T num_errors = 0;

    for (SIZE_T x = 0; x < width; x++)
    {
        if (block[top_row].str[x] != block[bottom_row].str[x])
        {
            num_errors++;
        }
    }

    return num_errors;
}
