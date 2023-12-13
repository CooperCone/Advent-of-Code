#include "solve.h"

#include "str.h"
#include "io.h"

typedef struct
{
    LONGLONG x;
    LONGLONG y;
} Point;

static BOOL dot_matrix[][3] =
{
    { FALSE, FALSE, FALSE },
    { FALSE, FALSE, FALSE },
    { FALSE, FALSE, FALSE },
};

static BOOL j_matrix[][3] =
{
    { FALSE, TRUE,  FALSE },
    { TRUE,  FALSE, FALSE },
    { FALSE, FALSE, FALSE },
};

static BOOL l_matrix[][3] =
{
    { FALSE, TRUE,  FALSE },
    { FALSE, FALSE, TRUE  },
    { FALSE, FALSE, FALSE },
};

static BOOL seven_matrix[][3] =
{
    { FALSE, FALSE, FALSE },
    { TRUE,  FALSE, FALSE },
    { FALSE, TRUE,  FALSE },
};

static BOOL f_matrix[][3] =
{
    { FALSE, FALSE, FALSE },
    { FALSE, FALSE, TRUE  },
    { FALSE, TRUE,  FALSE },
};

static BOOL dash_matrix[][3] =
{
    { FALSE, FALSE, FALSE },
    { TRUE,  FALSE, TRUE  },
    { FALSE, FALSE, FALSE },
};

static BOOL pipe_matrix[][3] =
{
    { FALSE, TRUE,  FALSE },
    { FALSE, FALSE, FALSE },
    { FALSE, TRUE,  FALSE },
};

static BOOL does_symbol_point_here(String *lines, Point start, Point offset);

static BOOL fill_next_points(Point start, LONGLONG width, LONGLONG num_lines,
    Point *out_points, SIZE_T *out_num_points);

Result10 solve10(LPCWSTR filename)
{
    Result10 result = {0};

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

    BOOL *is_on_loop = HeapAlloc(GetProcessHeap(),
        HEAP_ZERO_MEMORY, num_lines * width * sizeof *is_on_loop);

    if (is_on_loop == NULL)
    {
        goto CLEANUP_LINES;
    }

    Point start = {0};

    for (SIZE_T y = 0; y < num_lines; y++)
    {
        for (SIZE_T x = 0; x < width; x++)
        {
            if (lines[y].str[x] == 'S')
            {
                start.x = x;
                start.y = y;
            }
        }
    }

    Point points[4] = {0};
    SIZE_T num_points = 0;

    // Replace start symbol with what it's supposed to be
    if (!fill_next_points(start, width, num_lines, points, &num_points))
    {
        goto CLEANUP_IS_ON_LOOP;
    }

    BOOL is_top = FALSE;
    BOOL is_bot = FALSE;
    BOOL is_left = FALSE;
    BOOL is_right = FALSE;

    // Find which cells connect to the start
    for (SIZE_T i = 0; i < num_points; i++)
    {
        Point p = { start.x + points[i].x, start.y + points[i].y };
        Point rev_offset = { points[i].x * -1, points[i].y * -1 };

        if (does_symbol_point_here(lines, p, rev_offset))
        {
            if ((points[i].x == 0) && (points[i].y == 1))
            {
                is_bot = TRUE;
            }
            else if ((points[i].x == 0) && (points[i].y == -1))
            {
                is_top = TRUE;
            }
            else if ((points[i].x == 1) && (points[i].y == 0))
            {
                is_right = TRUE;
            }
            else if ((points[i].x == -1) && (points[i].y == 0))
            {
                is_left = TRUE;
            }
        }
    }

    // Now replace the start cell
    if (is_top && is_bot)
    {
        lines[start.y].str[start.x] = '|';
    }
    else if (is_left && is_right)
    {
        lines[start.y].str[start.x] = '-';
    }
    else if (is_left && is_bot)
    {
        lines[start.y].str[start.x] = '7';
    }
    else if (is_right && is_bot)
    {
        lines[start.y].str[start.x] = 'F';
    }
    else if (is_right && is_top)
    {
        lines[start.y].str[start.x] = 'L';
    }
    else if (is_left && is_top)
    {
        lines[start.y].str[start.x] = 'J';
    }

    Point last = { -1, -1 };
    Point path = start;

    BOOL first = TRUE;

    for (;;)
    {
        result.solve1++;

        if (result.solve1 > num_lines * width)
        {
            result.solve1 = 0;
            OutputDebugString(L"Not gonna happen\n");
            break;
        }

        is_on_loop[path.y * width + path.x] = TRUE;

        if (!first && (path.x == start.x) && (path.y == start.y))
        {
            break;
        }


        if (!fill_next_points(path, width, num_lines, points, &num_points))
        {
            goto CLEANUP_IS_ON_LOOP;
        }

        for (SIZE_T i = 0; i < num_points; i++)
        {
            Point offset = points[i];
            Point p =
            {
                path.x + offset.x,
                path.y + offset.y,
            };

            if (!((p.x == last.x) && (p.y == last.y)) &&
                (does_symbol_point_here(lines, path, offset)))
            {
                last = path;
                path = p;

                break;
            }
        }

        first = FALSE;
    }

    result.solve1 = result.solve1 / 2;

    for (SIZE_T y = 0; y < num_lines; y++)
    {
        for (SIZE_T x = 0; x < width; x++)
        {
            if (!is_on_loop[y * width + x])
            {
                lines[y].str[x] = '.';
            }
        }
    }

    for (SIZE_T y = 0; y < num_lines; y++)
    {
        BOOL is_in_loop = FALSE;
        BOOL saw_l = FALSE;
        BOOL saw_f = FALSE;

        for (SIZE_T x = 0; x < width; x++)
        {
            CHAR s = lines[y].str[x];

            switch (s)
            {
                case '.':
                    if (is_in_loop)
                    {
                        result.solve2++;
                    }
                    break;

                case '|':
                    is_in_loop = !is_in_loop;
                    break;

                case 'L':
                    saw_l = TRUE;
                    break;

                case '7':
                    if (saw_l)
                    {
                        saw_l = FALSE;
                        is_in_loop = !is_in_loop;
                    }
                    saw_f = FALSE;
                    break;

                case 'F':
                    saw_f = TRUE;
                    break;

                case 'J':
                    if (saw_f)
                    {
                        saw_f = FALSE;
                        is_in_loop = !is_in_loop;
                    }
                    saw_l = FALSE;
                    break;
            }
        }
    }

CLEANUP_IS_ON_LOOP:
    HeapFree(GetProcessHeap(), 0, is_on_loop);
    is_on_loop = NULL;

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

static BOOL does_symbol_point_here(String *lines, Point start, Point offset)
{
    CHAR symbol = lines[start.y].str[start.x];

    SIZE_T x = offset.x + 1;
    SIZE_T y = offset.y + 1;

    switch (symbol)
    {
        case '.':
            return dot_matrix[y][x];

        case '|':
            return pipe_matrix[y][x];

        case '-':
            return dash_matrix[y][x];

        case '7':
            return seven_matrix[y][x];

        case 'F':
            return f_matrix[y][x];

        case 'J':
            return j_matrix[y][x];

        case 'L':
            return l_matrix[y][x];
    }

    return FALSE;
}

static BOOL fill_next_points(Point start, LONGLONG width, LONGLONG num_lines,
    Point *out_points, SIZE_T *out_num_points)
{
    if ((out_points == NULL) || (out_num_points == NULL))
    {
        return FALSE;
    }

    Point points[] =
    {
        { -1,  0 },
        {  0, -1 },
        {  1,  0 },
        {  0,  1 },
    };

    SIZE_T num_points = 0;

    for (SIZE_T i = 0; i < 4; i++)
    {
        Point offset = points[i];
        Point p =
        {
            start.x + offset.x,
            start.y + offset.y
        };

        if ((p.x < 0) || (p.x >= width) || (p.y < 0) || (p.y >= num_lines))
        {
            continue;
        }

        out_points[num_points] = offset;
        num_points++;
    }

    *out_num_points = num_points;

    return TRUE;
}
