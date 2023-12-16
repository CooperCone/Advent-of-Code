#include "solve.h"

#include "str.h"
#include "io.h"

typedef struct
{
    BOOL is_energized;
    BOOL had_north;
    BOOL had_east;
    BOOL had_south;
    BOOL had_west;
} CellData;

static BOOL propagate_light(String *lines, SIZE_T width, SIZE_T height,
    SIZE_T x, SIZE_T y, LONGLONG dx, LONGLONG dy, CellData *cells);

static ULONGLONG calculate_energy(CellData *cells, SIZE_T width, SIZE_T height);

static BOOL clear_cell_memory(CellData *cells, SIZE_T width, SIZE_T height);

Result16 solve16(LPCWSTR filename)
{
    Result16 result = {0};

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

    SIZE_T width = lines[0].size;

    CellData *cells = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        width * height * sizeof *cells);

    if (cells == NULL)
    {
        goto CLEANUP_LINES;
    }

    for (SIZE_T y = 0; y < height; y++)
    {
        if (!clear_cell_memory(cells, width, height))
        {
            goto CLEANUP_CELLS;
        }

        if (!propagate_light(lines, width, height, 0, y, 1, 0, cells))
        {
            goto CLEANUP_CELLS;
        }

        ULONGLONG energy = calculate_energy(cells, width, height);
        result.solve2 = max(energy, result.solve2);

        if (y == 0)
        {
            result.solve1 = energy;
        }

        if (!clear_cell_memory(cells, width, height))
        {
            goto CLEANUP_CELLS;
        }

        if (!propagate_light(lines, width, height, width - 1, y, -1, 0, cells))
        {
            goto CLEANUP_CELLS;
        }

        energy = calculate_energy(cells, width, height);
        result.solve2 = max(energy, result.solve2);
    }

    for (SIZE_T x = 0; x < width; x++)
    {
        if (!clear_cell_memory(cells, width, height))
        {
            goto CLEANUP_CELLS;
        }

        if (!propagate_light(lines, width, height, x, 0, 0, 1, cells))
        {
            goto CLEANUP_CELLS;
        }

        ULONGLONG energy = calculate_energy(cells, width, height);
        result.solve2 = max(energy, result.solve2);

        if (!clear_cell_memory(cells, width, height))
        {
            goto CLEANUP_CELLS;
        }

        if (!propagate_light(lines, width, height, x, height - 1, 0, -1, cells))
        {
            goto CLEANUP_CELLS;
        }

        energy = calculate_energy(cells, width, height);
        result.solve2 = max(energy, result.solve2);
    }

CLEANUP_CELLS:
    HeapFree(GetProcessHeap(), 0, cells);
    cells = NULL;

CLEANUP_LINES:
    HeapFree(GetProcessHeap(), 0, lines);
    lines = NULL;

CLEANUP_FILE_DATA:
    HeapFree(GetProcessHeap(), 0, file_data);
    file_data = NULL;

COMPLETE:
    return result;
}

static BOOL propagate_light(String *lines, SIZE_T width, SIZE_T height,
    SIZE_T x, SIZE_T y, LONGLONG dx, LONGLONG dy, CellData *cells)
{
    if ((lines == NULL) || (width == 0) || (height == 0))
    {
        return FALSE;
    }

    if ((x < 0) || (y < 0) || (x >= width) || (y >= height))
    {
        return TRUE;
    }

    BOOL result = FALSE;

    CellData cell_data = cells[y * width + x];

    if (((dx ==  0) && (dy == -1) && (cell_data.had_north)) ||
         (dx ==  1) && (dy ==  0) && (cell_data.had_east) ||
         (dx ==  0) && (dy ==  1) && (cell_data.had_south) ||
         (dx == -1) && (dy ==  0) && (cell_data.had_west))
    {
        return TRUE;
    }

    cells[y * width + x].is_energized = TRUE;

    if ((dx ==  0) && (dy == -1))
    {
        cells[y * width + x].had_north = TRUE;
    }
    else if ((dx ==  1) && (dy ==  0))
    {
        cells[y * width + x].had_east = TRUE;
    }
    else if ((dx ==  0) && (dy ==  1))
    {
        cells[y * width + x].had_south = TRUE;
    }
    else if ((dx == -1) && (dy ==  0))
    {
        cells[y * width + x].had_west = TRUE;
    }

    CHAR cell = lines[y].str[x];

    if ((cell == '|') && (dy == 0))
    {
        result = propagate_light(lines, width, height, x, y - 1, 0, -1, cells) |
            propagate_light(lines, width, height, x, y + 1, 0, 1, cells);
    }
    else if ((cell == '-') && (dx == 0))
    {
        result = propagate_light(lines, width, height, x - 1, y, -1, 0, cells) |
            propagate_light(lines, width, height, x + 1, y, 1, 0, cells);
    }
    else
    {
        if (cell == '/')
        {
            LONGLONG tmp_dx = dx;
            dx = -dy;
            dy = -tmp_dx;
        }
        else if (cell == '\\')
        {
            LONGLONG tmp_dx = dx;
            dx = dy;
            dy = tmp_dx;
        }

        result = propagate_light(lines, width, height, x + dx, y + dy,
            dx, dy, cells);
    }

    return result;
}

static ULONGLONG calculate_energy(CellData *cells, SIZE_T width, SIZE_T height)
{
    if ((cells == NULL) || (width == 0) || (height == 0))
    {
        return 0;
    }

    ULONGLONG result = 0;

    for (SIZE_T y = 0; y < height; y++)
    {
        for (SIZE_T x = 0; x < width; x++)
        {
            if (cells[y * width + x].is_energized)
            {
                result++;
            }
        }
    }

    return result;
}

static BOOL clear_cell_memory(CellData *cells, SIZE_T width, SIZE_T height)
{
    if ((cells == NULL) || (width == 0) || (height == 0))
    {
        return FALSE;
    }

    for (SIZE_T y = 0; y < height; y++)
    {
        for (SIZE_T x = 0; x < width; x++)
        {
            cells[y * width + x] = (CellData){0};
        }
    }

    return TRUE;
}
