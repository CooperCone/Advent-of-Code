#include "solve.h"

#include "str.h"
#include "io.h"

static BOOL calculate_empty_rows(String *lines, SIZE_T width, SIZE_T num_lines,
    SIZE_T *num_empty_rows);

static BOOL calculate_empty_cols(String *lines, SIZE_T width, SIZE_T num_lines,
    SIZE_T *num_empty_cols);

static SIZE_T calculate_all_distances(String *lines, SIZE_T width, SIZE_T height,
    SIZE_T start_x, SIZE_T start_y, SIZE_T *num_empty_rows,
        SIZE_T *num_empty_cols, SIZE_T space_modifier);

static SIZE_T manhattan_dist(SIZE_T x1, SIZE_T y1, SIZE_T x2, SIZE_T y2);

Result11 solve11(LPCWSTR filename)
{
    Result11 result = {0};

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

    SIZE_T num_lines = 0;
    SIZE_T max_lines = 8;
    String *lines = HeapAlloc(GetProcessHeap(), 0, max_lines * sizeof *lines);

    if (lines == NULL)
    {
        goto CLEANUP_FILE_DATA;
    }

    for (;;)
    {
        String line = tokenize(&file_str, "\r\n");

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
                goto CLEANUP_LINES;
            }

            lines = new_lines;
        }

        lines[num_lines - 1] = line;
    }

    SIZE_T width = lines[0].size;

    SIZE_T *num_empty_rows = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        width * sizeof *num_empty_rows);

    if (num_empty_rows == NULL)
    {
        goto CLEANUP_LINES;
    }

    SIZE_T *num_empty_cols = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        num_lines * sizeof *num_empty_cols);

    if (num_empty_rows == NULL)
    {
        goto CLEANUP_EMPTY_ROWS;
    }

    if (!calculate_empty_rows(lines, width, num_lines, num_empty_rows))
    {
        goto CLEANUP_EMPTY_COLS;
    }

    if (!calculate_empty_cols(lines, width, num_lines, num_empty_cols))
    {
        goto CLEANUP_EMPTY_COLS;
    }

    for (SIZE_T y = 0; y < num_lines; y++)
    {
        for (SIZE_T x = 0; x < width; x++)
        {
            if (lines[y].str[x] == '#')
            {
                lines[y].str[x] = '@';

                result.solve1 += calculate_all_distances(lines, width,
                    num_lines, x, y, num_empty_rows, num_empty_cols, 2);

                result.solve2 += calculate_all_distances(lines, width,
                    num_lines, x, y, num_empty_rows, num_empty_cols, 1000000);
            }
        }
    }

CLEANUP_EMPTY_COLS:
    HeapFree(GetProcessHeap(), 0, num_empty_cols);
    num_empty_cols = NULL;

CLEANUP_EMPTY_ROWS:
    HeapFree(GetProcessHeap(), 0, num_empty_rows);
    num_empty_rows = NULL;

CLEANUP_LINES:
    HeapFree(GetProcessHeap(), 0, lines);
    lines = NULL;
    num_lines = 0;

CLEANUP_FILE_DATA:
    HeapFree(GetProcessHeap(), 0, file_data);
    file_data = NULL;

COMPLETE:
    return result;
}

static BOOL calculate_empty_rows_cols(String *lines, SIZE_T width,
    SIZE_T num_lines, PSIZE_T out_empty_cols, PSIZE_T out_empty_rows)
{
    if ((lines == NULL) || (width == 0) || (num_lines == 0) ||
        (out_empty_cols == NULL) || (out_empty_rows == NULL))
    {
        return FALSE;
    }

    SIZE_T num_empty_columns = 0;

    for (SIZE_T x = 0; x < width; x++)
    {
        BOOL found_galaxy = FALSE;

        for (SIZE_T y = 0; y < num_lines; y++)
        {
            if (lines[y].str[x] == '#')
            {
                found_galaxy = TRUE;
                break;
            }
        }

        if (!found_galaxy)
        {
            num_empty_columns++;
        }
    }

    SIZE_T num_empty_rows = 0;

    for (SIZE_T y = 0; y < num_lines; y++)
    {
        BOOL found_galaxy = FALSE;

        for (SIZE_T x = 0; x < width; x++)
        {
            if (lines[y].str[x] == '#')
            {
                found_galaxy = TRUE;
                break;
            }
        }

        if (!found_galaxy)
        {
            num_empty_rows++;
        }
    }

    *out_empty_cols = num_empty_columns;
    *out_empty_rows = num_empty_rows;

    return TRUE;
}

static BOOL calculate_empty_rows(String *lines, SIZE_T width, SIZE_T num_lines,
    SIZE_T *num_empty_rows)
{
    if ((lines == NULL) || (width == 0) || (num_lines == 0) ||
        (num_empty_rows == NULL))
    {
        return FALSE;
    }

    SIZE_T total_num_empty_rows = 0;

    for (SIZE_T y = 0; y < num_lines; y++)
    {
        BOOL is_empty = TRUE;

        for (SIZE_T x = 0; x < width; x++)
        {
            if (lines[y].str[x] == '#')
            {
                is_empty = FALSE;
                break;
            }
        }

        if (is_empty)
        {
            total_num_empty_rows++;
        }

        num_empty_rows[y] = total_num_empty_rows;
    }

    return TRUE;
}

static BOOL calculate_empty_cols(String *lines, SIZE_T width, SIZE_T num_lines,
    SIZE_T *num_empty_cols)
{
    if ((lines == NULL) || (width == 0) || (num_lines == 0) ||
        (num_empty_cols == NULL))
    {
        return FALSE;
    }

    SIZE_T total_num_empty_cols = 0;

    for (SIZE_T x = 0; x < width; x++)
    {
        BOOL is_empty = TRUE;

        for (SIZE_T y = 0; y < num_lines; y++)
        {
            if (lines[y].str[x] == '#')
            {
                is_empty = FALSE;
                break;
            }
        }

        if (is_empty)
        {
            total_num_empty_cols++;
        }

        num_empty_cols[x] = total_num_empty_cols;
    }

    return TRUE;
}

static SIZE_T calculate_all_distances(String *lines, SIZE_T width, SIZE_T height,
    SIZE_T start_x, SIZE_T start_y, SIZE_T *num_empty_rows,
        SIZE_T *num_empty_cols, SIZE_T space_modifier)
{
    if ((lines == NULL) || (width == 0) || (height == 0))
    {
        return 0;
    }

    SIZE_T total_distances = 0;

    for (SIZE_T y = 0; y < height; y++)
    {
        for (SIZE_T x = 0; x < width; x++)
        {
            if (lines[y].str[x] == '#')
            {
                SIZE_T min_y = min(start_y, y);
                SIZE_T max_y = max(start_y, y);
                SIZE_T min_x = min(start_x, x);
                SIZE_T max_x = max(start_x, x);

                total_distances += manhattan_dist(start_x, start_y, x, y) +
                    (space_modifier - 1) *
                        (num_empty_rows[max_y] - num_empty_rows[min_y]) +
                    (space_modifier - 1) *
                        (num_empty_cols[max_x] - num_empty_cols[min_x]);
            }
        }
    }

    return total_distances;
}

static SIZE_T manhattan_dist(SIZE_T x1, SIZE_T y1, SIZE_T x2, SIZE_T y2)
{
    SIZE_T ydiff = y2 > y1 ? y2 - y1 : y1 - y2;
    SIZE_T xdiff = x2 > x1 ? x2 - x1 : x1 - x2;

    return ydiff + xdiff;
}
