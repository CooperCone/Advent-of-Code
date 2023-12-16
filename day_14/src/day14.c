#include "solve.h"

#include "str.h"
#include "io.h"

#define NUM_ITERS 1000000000

static BOOL rotate_north(String *lines, SIZE_T width, SIZE_T height);

static BOOL rotate_east(String *lines, SIZE_T width, SIZE_T height);

static BOOL rotate_south(String *lines, SIZE_T width, SIZE_T height);

static BOOL rotate_west(String *lines, SIZE_T width, SIZE_T height);

static BOOL calculate_load(String *lines, SIZE_T width, SIZE_T height,
    ULONGLONG *out_load);

static BOOL search_stored_lines(String *lines, SIZE_T width, SIZE_T height,
    PBYTE stored_lines, SIZE_T num_lines, SIZE_T *out_num_line);

static BOOL append_lines(String *lines, SIZE_T width, SIZE_T height,
    PBYTE *out_stored_lines, SIZE_T *out_num_lines);

Result14 solve14(LPCWSTR filename)
{
    Result14 result = {0};

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

    String *lines = NULL;
    SIZE_T height = 0;

    if (!split(&file_str, "\r\n", &lines, &height))
    {
        goto CLEANUP_FILE_DATA;
    }

    SIZE_T num_lines = 0;
    PBYTE stored_lines = NULL;

    SIZE_T width = lines[0].size;

    for (SIZE_T i = 0; i < NUM_ITERS; i++)
    {
        if (!rotate_north(lines, width, height))
        {
            goto CLEANUP_STORED_LINES;
        }

        if (i == 0)
        {
            if (!calculate_load(lines, width, height, &result.solve1))
            {
                goto CLEANUP_STORED_LINES;
            }
        }

        if (!rotate_west(lines, width, height))
        {
            goto CLEANUP_STORED_LINES;
        }

        if (!rotate_south(lines, width, height))
        {
            goto CLEANUP_STORED_LINES;
        }

        if (!rotate_east(lines, width, height))
        {
            goto CLEANUP_STORED_LINES;
        }

        SIZE_T num_line = 0;

        if (search_stored_lines(lines, width, height, stored_lines, num_lines,
                &num_line))
        {
            SIZE_T diff = i - num_line;

            while (i + diff < NUM_ITERS)
            {
                i += diff;
            }
        }

        if (!append_lines(lines, width, height, &stored_lines, &num_lines))
        {
            goto CLEANUP_STORED_LINES;
        }
    }

    if (!calculate_load(lines, width, height, &result.solve2))
    {
        goto CLEANUP_STORED_LINES;
    }

CLEANUP_STORED_LINES:
    HeapFree(GetProcessHeap(), 0, stored_lines);
    stored_lines = NULL;

    HeapFree(GetProcessHeap(), 0, lines);
    lines = NULL;
    height = 0;
    width = 0;

CLEANUP_FILE_DATA:
    HeapFree(GetProcessHeap(), 0, file_data);
    file_data = NULL;

COMPLETE:
    return result;
}

static BOOL rotate_north(String *lines, SIZE_T width, SIZE_T height)
{
    if ((lines == NULL) || (width == 0) || (height == 0))
    {
        return FALSE;
    }

    for (SIZE_T y = 0; y < height; y++)
    {
        for (SIZE_T x = 0; x < width; x++)
        {
            if (lines[y].str[x] == 'O')
            {
                // Move rock north
                for (LONGLONG new_y = y - 1; new_y > -1; new_y--)
                {
                    if (lines[new_y].str[x] == '.')
                    {
                        lines[new_y].str[x] = 'O';
                        lines[new_y + 1].str[x] = '.';
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }

    return TRUE;
}

static BOOL rotate_east(String *lines, SIZE_T width, SIZE_T height)
{
    if ((lines == NULL) || (width == 0) || (height == 0))
    {
        return FALSE;
    }

    for (LONGLONG x = width - 1; x > -1; x--)
    {
        for (SIZE_T y = 0; y < height; y++)
        {
            if (lines[y].str[x] == 'O')
            {
                // Move rock east
                for (LONGLONG new_x = x + 1; new_x < (LONGLONG)width; new_x++)
                {
                    if (lines[y].str[new_x] == '.')
                    {
                        lines[y].str[new_x] = 'O';
                        lines[y].str[new_x - 1] = '.';
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }

    return TRUE;
}

static BOOL rotate_south(String *lines, SIZE_T width, SIZE_T height)
{
    if ((lines == NULL) || (width == 0) || (height == 0))
    {
        return FALSE;
    }

    for (LONGLONG y = height - 1; y > -1; y--)
    {
        for (SIZE_T x = 0; x < width; x++)
        {
            if (lines[y].str[x] == 'O')
            {
                // Move rock south
                for (SIZE_T new_y = y + 1; new_y < height; new_y++)
                {
                    if (lines[new_y].str[x] == '.')
                    {
                        lines[new_y].str[x] = 'O';
                        lines[new_y - 1].str[x] = '.';
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }

    return TRUE;
}

static BOOL rotate_west(String *lines, SIZE_T width, SIZE_T height)
{
    if ((lines == NULL) || (width == 0) || (height == 0))
    {
        return FALSE;
    }

    for (SIZE_T x = 0; x < width; x++)
    {
        for (SIZE_T y = 0; y < height; y++)
        {
            if (lines[y].str[x] == 'O')
            {
                // Move rock east
                for (LONGLONG new_x = x - 1; new_x > -1; new_x--)
                {
                    if (lines[y].str[new_x] == '.')
                    {
                        lines[y].str[new_x] = 'O';
                        lines[y].str[new_x + 1] = '.';
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }

    return TRUE;
}

static BOOL calculate_load(String *lines, SIZE_T width, SIZE_T height,
    ULONGLONG *out_load)
{
    if ((lines == NULL) || (width == 0) || (height == 0) || (out_load == NULL))
    {
        return FALSE;
    }

    *out_load = 0;

    for (SIZE_T y = 0; y < height; y++)
    {
        SIZE_T row_load = height - y;

        for (SIZE_T x = 0; x < width; x++)
        {
            if (lines[y].str[x] == 'O')
            {
                *out_load += row_load;
            }
        }
    }

    return TRUE;
}

static BOOL search_stored_lines(String *lines, SIZE_T width, SIZE_T height,
    PBYTE stored_lines, SIZE_T num_lines, SIZE_T *out_num_line)
{
    if ((lines == NULL) || (width == 0) || (height == 0) ||
        (stored_lines == NULL) || (num_lines == 0) || (out_num_line == NULL))
    {
        return FALSE;
    }

    BOOL found_it = TRUE;

    for (SIZE_T i = 0; i < num_lines; i++)
    {
        found_it = TRUE;

        PBYTE cur_lines = stored_lines + i * width * height;

        for (SIZE_T y = 0; y < height; y++)
        {
            for (SIZE_T x = 0; x < width; x++)
            {
                if (lines[y].str[x] != cur_lines[y * width + x])
                {
                    found_it = FALSE;
                }
            }

            if (!found_it)
            {
                break;
            }
        }

        if (found_it)
        {
            *out_num_line = i;
            break;
        }
    }

    return found_it;
}

static BOOL append_lines(String *lines, SIZE_T width, SIZE_T height,
    PBYTE *out_stored_lines, SIZE_T *out_num_stored)
{
    if ((lines == NULL) || (width == 0) || (height == 0) ||
        (out_stored_lines == NULL) || (out_num_stored == NULL))
    {
        return FALSE;
    }

    BOOL result = FALSE;

    PBYTE new_stored_lines = *out_stored_lines;
    SIZE_T new_size = *out_num_stored + 1;

    if (new_size == 1)
    {
        new_stored_lines = HeapAlloc(GetProcessHeap(), 0,
            new_size * width * height);

        if (new_stored_lines == NULL)
        {
            goto COMPLETE;
        }
    }
    else
    {
        PBYTE new_lines = HeapReAlloc(GetProcessHeap(), 0, new_stored_lines,
            new_size * width * height);

        if (new_lines == NULL)
        {
            goto COMPLETE;
        }

        new_stored_lines = new_lines;
    }

    PBYTE cur_lines = new_stored_lines + *out_num_stored * width * height;

    for (SIZE_T y = 0; y < height; y++)
    {
        if (!copy_memory(cur_lines + y * width, lines[y].str, width))
        {
            goto COMPLETE;
        }
    }

    *out_stored_lines = new_stored_lines;
    *out_num_stored = new_size;

    result = TRUE;

COMPLETE:
    return result;
}
