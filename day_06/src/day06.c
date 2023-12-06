#include "solve.h"

#include "str.h"
#include "io.h"

static SIZE_T num_races = 0;
static PULONGLONG times = NULL;
static PULONGLONG distances = NULL;

static BOOL parse_file(PBYTE file_data, SIZE_T file_size);

static ULONGLONG calculate_ways_to_win(ULONGLONG time, ULONGLONG required_dist);

Result06 solve06(LPCWSTR filename)
{
    Result06 result = {0};

    SIZE_T file_size = 0;

    PBYTE file_data = read_file(filename, &file_size);

    if (file_data == NULL)
    {
        goto COMPLETE;
    }

    if (!parse_file(file_data, file_size))
    {
        goto CLEANUP_RACES;
    }

    for (SIZE_T i = 0; i < num_races; i++)
    {
        ULONGLONG ways_to_win = calculate_ways_to_win(times[i], distances[i]);

        if ((result.solve1 == 0) && (ways_to_win > 0))
        {
            result.solve1 = 1;
        }

        result.solve1 *= ways_to_win;
    }

    ULONGLONG num_time_digits = 0;
    ULONGLONG num_dist_digits = 0;
    ULONGLONG single_time = 0;
    ULONGLONG single_dist = 0;

    for (SSIZE_T i = num_races - 1; i >= 0 ; i--)
    {
        ULONGLONG time = times[i];
        ULONGLONG dist = distances[i];

        while (time > 0)
        {
            ULONGLONG time_digit = time % 10;
            ULONGLONG time_remaining = time / 10;

            for (SIZE_T j = 0; j < num_time_digits; j++)
            {
                time_digit *= 10;
            }

            single_time += time_digit;
            time = time_remaining;
            num_time_digits++;
        }

        while (dist > 0)
        {
            ULONGLONG dist_digit = dist % 10;
            ULONGLONG dist_remaining = dist / 10;

            for (SIZE_T j = 0; j < num_dist_digits; j++)
            {
                dist_digit *= 10;
            }

            single_dist += dist_digit;
            dist = dist_remaining;
            num_dist_digits++;
        }
    }

    result.solve2 = calculate_ways_to_win(single_time, single_dist);

CLEANUP_RACES:
    num_races = 0;

    HeapFree(GetProcessHeap(), 0, times);
    times = NULL;

    HeapFree(GetProcessHeap(), 0, distances);
    distances = NULL;

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

    String time = tokenize(&file_str, "\r\n");
    String distance = tokenize(&file_str, "\r\n");

    if ((time.str == NULL) || (distance.str == NULL))
    {
        goto COMPLETE;
    }

    if (!consume_if(&time, "Time:"))
    {
        goto COMPLETE;
    }

    if (!consume_if(&distance, "Distance:"))
    {
        goto COMPLETE;
    }

    while (time.size > 0)
    {
        while (consume_if(&time, " "));
        while (consume_if(&distance, " "));

        num_races++;

        SIZE_T num_bytes = num_races * sizeof(ULONGLONG);

        if (num_races == 1)
        {
            times = HeapAlloc(GetProcessHeap(), 0, num_bytes);
            distances = HeapAlloc(GetProcessHeap(), 0, num_bytes);

            if ((times == NULL) || (distances == NULL))
            {
                goto COMPLETE;
            }
        }
        else
        {
            PULONGLONG new_times = HeapReAlloc(GetProcessHeap(), 0, times,
                num_bytes);
            PULONGLONG new_distances = HeapReAlloc(GetProcessHeap(), 0,
                distances, num_bytes);

            if ((new_times == NULL) || (new_distances == NULL))
            {
                goto COMPLETE;
            }

            times = new_times;
            distances = new_distances;
        }

        if (!parse_uint(&time, times + num_races - 1))
        {
            goto COMPLETE;
        }

        if (!parse_uint(&distance, distances + num_races - 1))
        {
            goto COMPLETE;
        }
    }

    result = TRUE;

COMPLETE:
    return result;
}

static ULONGLONG calculate_ways_to_win(ULONGLONG time, ULONGLONG required_dist)
{
    ULONGLONG ways_to_win = 0;

    for (SIZE_T j = 0; j < time; j++)
    {
        ULONGLONG travelled_dist = j * (time - j);

        if (travelled_dist > required_dist)
        {
            ways_to_win++;
        }
    }

    return ways_to_win;
}
