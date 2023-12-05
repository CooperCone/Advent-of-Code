#include "solve.h"

#include "str.h"
#include "io.h"

typedef struct
{
    ULONGLONG dst_start;
    ULONGLONG src_start;
    ULONGLONG size;
} Range;

typedef struct
{
    SIZE_T num_ranges;
    SIZE_T max_ranges;
    Range *ranges;
} RangeList;

static BOOL parse_file(PBYTE file_data, SIZE_T file_size);

static BOOL parse_range(String string, Range *out_range);

static BOOL append_range(RangeList *list, Range *range);

static BOOL break_ranges(RangeList *seed_ranges, RangeList *range_map);

static BOOL map_ranges(RangeList *seed_ranges, RangeList *range_map);

static BOOL map_all_ranges(RangeList *ranges);

static BOOL find_min(RangeList *ranges, PULONGLONG out_value);

static BOOL solve1(PULONGLONG solve_value);

static BOOL solve2(PULONGLONG solve_value);

static SIZE_T num_seeds;
static ULONGLONG *seeds;

static RangeList range_maps[7];

Result05 solve05(LPCWSTR filename)
{
    Result05 result = {0};

    SIZE_T file_size = 0;

    PBYTE file_data = read_file(filename, &file_size);

    if (file_data == NULL)
    {
        goto COMPLETE;
    }

    if (!parse_file(file_data, file_size))
    {
        goto CLEANUP_SEEDS;
    }

    if (!solve1(&result.solve1))
    {
        goto CLEANUP_SEEDS;
    }

    if (!solve2(&result.solve2))
    {
        goto CLEANUP_SEEDS;
    }

CLEANUP_SEEDS:
    HeapFree(GetProcessHeap(), 0, seeds);
    seeds = NULL;
    num_seeds = 0;

    for (SIZE_T i = 0; i < ARRAY_SIZE(range_maps); i++)
    {
        HeapFree(GetProcessHeap(), 0, range_maps[0].ranges);
        range_maps[0] = (RangeList){0};
    }

    HeapFree(GetProcessHeap(), 0, file_data);
    file_data = NULL;

COMPLETE:
    return result;
}

static BOOL parse_file(PBYTE file_data, SIZE_T file_size)
{
    if ((file_data == NULL) || (file_size == 0))
    {
        return FALSE;
    }

    BOOL result = FALSE;

    String file_str =
    {
        .str = file_data,
        .size = file_size
    };

    String seed_line = tokenize(&file_str, "\r\n");

    if (seed_line.size == 0)
    {
        goto COMPLETE;
    }

    if (!consume_if(&seed_line, "seeds: "))
    {
        goto COMPLETE;
    }

    while (seed_line.size > 0)
    {
        ULONGLONG seed = 0;

        if (!parse_uint(&seed_line, &seed))
        {
            goto COMPLETE;
        }

        num_seeds++;

        if (num_seeds == 1)
        {
            seeds = HeapAlloc(GetProcessHeap(), 0, num_seeds * sizeof *seeds);

            if (seeds == NULL)
            {
                goto COMPLETE;
            }
        }
        else
        {
            ULONGLONG *new_seeds = HeapReAlloc(GetProcessHeap(), 0, seeds,
                num_seeds * sizeof *seeds);

            if (new_seeds == NULL)
            {
                goto COMPLETE;
            }

            seeds = new_seeds;
        }

        seeds[num_seeds - 1] = seed;

        consume_if(&seed_line, " ");
    }

    SIZE_T map_num = 0;

    String line = tokenize(&file_str, "\r\n");

    while ((line.str != NULL) && (map_num < ARRAY_SIZE(range_maps)))
    {
        line = tokenize(&file_str, "\r\n");
        line = tokenize(&file_str, "\r\n");

        while ((line.size > 0) && is_digit(line.str[0]))
        {
            Range range = {0};

            if (!parse_range(line, &range))
            {
                goto COMPLETE;
            }

            if (!append_range(range_maps + map_num, &range))
            {
                goto COMPLETE;
            }

            line = tokenize(&file_str, "\r\n");
        }

        map_num++;
    }

    result = TRUE;

COMPLETE:
    return result;
}

static BOOL parse_range(String string, Range *out_range)
{
    if ((string.str == NULL) || (string.size == 0) || (out_range == NULL))
    {
        return FALSE;
    }

    BOOL result = FALSE;

    ULONGLONG dst = 0;

    if (!parse_uint(&string, &dst))
    {
        goto COMPLETE;
    }

    if (!consume_if(&string, " "))
    {
        goto COMPLETE;
    }

    ULONGLONG src = 0;

    if (!parse_uint(&string, &src))
    {
        goto COMPLETE;
    }

    if (!consume_if(&string, " "))
    {
        goto COMPLETE;
    }

    ULONGLONG size = 0;

    if (!parse_uint(&string, &size))
    {
        goto COMPLETE;
    }

    *out_range = (Range)
    {
        .dst_start = dst,
        .src_start = src,
        .size = size
    };

    result = TRUE;

COMPLETE:
    return result;
}

static BOOL append_range(RangeList *list, Range *range)
{
    if ((list == NULL) || (list->num_ranges > list->max_ranges) ||
        (range == NULL))
    {
        return FALSE;
    }

    BOOL result = FALSE;

    if (list->num_ranges == list->max_ranges)
    {
        if (list->max_ranges == 0)
        {
            list->max_ranges = 8;

            list->ranges = HeapAlloc(GetProcessHeap(), 0,
                list->max_ranges * sizeof *list->ranges);

            if (list->ranges == NULL)
            {
                goto COMPLETE;
            }
        }
        else
        {
            list->max_ranges *= 2;

            Range *new_ranges = HeapReAlloc(GetProcessHeap(), 0,
                list->ranges, list->max_ranges * sizeof *list->ranges);

            if (new_ranges == NULL)
            {
                goto COMPLETE;
            }

            list->ranges = new_ranges;
        }
    }

    list->ranges[list->num_ranges] = *range;
    list->num_ranges++;

    result = TRUE;

COMPLETE:
    return result;
}

static BOOL break_ranges(RangeList *seed_ranges, RangeList *range_map)
{
    if ((seed_ranges == NULL) || (seed_ranges->ranges == NULL) ||
        (seed_ranges->num_ranges > seed_ranges->max_ranges) ||
        (range_map == NULL) || (range_map->num_ranges > range_map->max_ranges))
    {
        return FALSE;
    }

    BOOL result = FALSE;

    BOOL ranges_changed = TRUE;

    while (ranges_changed)
    {
        ranges_changed = FALSE;

        for (SIZE_T map_idx = 0; map_idx < range_map->num_ranges; map_idx++)
        {
            Range map_range = range_map->ranges[map_idx];

            ULONGLONG map_start = map_range.src_start;
            ULONGLONG map_end = map_range.src_start + map_range.size - 1;

            for (SIZE_T seed_idx = 0; seed_idx < seed_ranges->num_ranges;
                seed_idx++)
            {
                Range *seed_range = seed_ranges->ranges + seed_idx;

                ULONGLONG seed_start = seed_range->src_start;
                ULONGLONG seed_end = seed_range->src_start +
                    seed_range->size - 1;

                if ((seed_start < map_start) && (seed_end >= map_start))
                {
                    // Break off left side
                    Range left_range =
                    {
                        .src_start = seed_start,
                        .size = map_start - seed_start
                    };

                    seed_range->src_start += left_range.size;
                    seed_range->size -= left_range.size;

                    if (!append_range(seed_ranges, &left_range))
                    {
                        goto COMPLETE;
                    }

                    ranges_changed = TRUE;
                    break;
                }

                if ((seed_start <= map_end) && (seed_end > map_end))
                {
                    // Break off right side
                    Range right_range =
                    {
                        .src_start = map_end + 1,
                        .size = seed_end - map_end
                    };

                    seed_range->size -= right_range.size;

                    if (!append_range(seed_ranges, &right_range))
                    {
                        goto COMPLETE;
                    }

                    ranges_changed = TRUE;
                    break;
                }
            }

            if (ranges_changed)
            {
                break;
            }
        }
    }

    result = TRUE;

COMPLETE:
    return result;
}

static BOOL map_ranges(RangeList *seed_ranges, RangeList *range_map)
{
    if ((seed_ranges == NULL) || (seed_ranges->ranges == NULL) ||
        (seed_ranges->num_ranges > seed_ranges->max_ranges) ||
        (range_map == NULL) || (range_map->num_ranges > range_map->max_ranges))
    {
        return FALSE;
    }

    for (SIZE_T map_idx = 0; map_idx < range_map->num_ranges; map_idx++)
    {
        Range map_range = range_map->ranges[map_idx];

        for (SIZE_T seed_idx = 0; seed_idx < seed_ranges->num_ranges;
            seed_idx++)
        {
            Range *seed_range = seed_ranges->ranges + seed_idx;

            if ((seed_range->src_start >= map_range.src_start) &&
                (seed_range->src_start + seed_range->size <=
                 map_range.src_start + map_range.size))
            {
                seed_range->dst_start = seed_range->src_start -
                    map_range.src_start + map_range.dst_start;
            }
        }
    }

    return TRUE;
}

static BOOL map_all_ranges(RangeList *ranges)
{
    if ((ranges == NULL) || (ranges->ranges == NULL) ||
        (ranges->num_ranges > ranges->max_ranges))
    {
        return FALSE;
    }

    BOOL result = FALSE;

    for (SIZE_T i = 0; i < ARRAY_SIZE(range_maps); i++)
    {
        if (!break_ranges(ranges, range_maps + i))
        {
            goto COMPLETE;
        }

        for (SIZE_T j = 0; j < ranges->num_ranges; j++)
        {
            ranges->ranges[j].dst_start = ranges->ranges[j].src_start;
        }

        if (!map_ranges(ranges, range_maps + i))
        {
            goto COMPLETE;
        }

        for (SIZE_T j = 0; j < ranges->num_ranges; j++)
        {
            ranges->ranges[j].src_start = ranges->ranges[j].dst_start;
            ranges->ranges[j].dst_start = 0;
        }
    }

    result = TRUE;

COMPLETE:
    return result;
}

static BOOL find_min(RangeList *ranges, PULONGLONG out_value)
{
    if ((ranges == NULL) || (ranges->ranges == NULL) ||
        (ranges->num_ranges > ranges->max_ranges))
    {
        return FALSE;
    }

    ULONGLONG min_value = ULLONG_MAX;

    for (SIZE_T i = 0; i < ranges->num_ranges; i++)
    {
        ULONGLONG location = ranges->ranges[i].src_start;

        if (location < min_value)
        {
            min_value = location;
        }
    }

    *out_value = min_value;

    return TRUE;
}

static BOOL solve1(PULONGLONG solve_value)
{
    if (solve_value == NULL)
    {
        return FALSE;
    }

    BOOL result = FALSE;

    RangeList seed_ranges = {0};

    for (SIZE_T i = 0; i < num_seeds; i++)
    {
        Range range =
        {
            .src_start = seeds[i],
            .size = 1
        };

        if (!append_range(&seed_ranges, &range))
        {
            goto CLEANUP_SEED_RANGES;
        }
    }

    if (!map_all_ranges(&seed_ranges))
    {
        goto CLEANUP_SEED_RANGES;
    }

    if (!find_min(&seed_ranges, solve_value))
    {
        goto CLEANUP_SEED_RANGES;
    }

    result = TRUE;

CLEANUP_SEED_RANGES:
    HeapFree(GetProcessHeap(), 0, seed_ranges.ranges);
    seed_ranges = (RangeList){0};

COMPLETE:
    return result;
}

static BOOL solve2(PULONGLONG solve_value)
{
    if (solve_value == NULL)
    {
        return FALSE;
    }

    BOOL result = FALSE;

    RangeList seed_ranges = {0};

    for (SIZE_T i = 0; i < num_seeds / 2; i++)
    {
        Range range =
        {
            .src_start = seeds[i * 2],
            .size = seeds[i * 2 + 1]
        };

        if (!append_range(&seed_ranges, &range))
        {
            goto CLEANUP_SEED_RANGES;
        }
    }

    if (!map_all_ranges(&seed_ranges))
    {
        goto CLEANUP_SEED_RANGES;
    }

    if (!find_min(&seed_ranges, solve_value))
    {
        goto CLEANUP_SEED_RANGES;
    }

    result = TRUE;

CLEANUP_SEED_RANGES:
    HeapFree(GetProcessHeap(), 0, seed_ranges.ranges);
    seed_ranges = (RangeList){0};

COMPLETE:
    return result;
}
