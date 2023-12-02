#include "solve.h"

#include "str.h"
#include "io.h"

Result02 solve02(LPCWSTR filename)
{
    Result02 result = {0};

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

        if (!consume(&line, cstrlen("Game ")))
        {
            goto CLEANUP_FILE;
        }

        ULONGLONG game_id = 0;

        if (!parse_uint(&line, &game_id))
        {
            goto CLEANUP_FILE;
        }

        if (!consume(&line, cstrlen(": ")))
        {
            goto CLEANUP_FILE;
        }

        ULONGLONG num_reds = 0;
        ULONGLONG num_greens = 0;
        ULONGLONG num_blues = 0;

        while (line.size > 0)
        {
            while ((line.size > 0) && (*line.str != ';'))
            {
                ULONGLONG number = 0;

                if (!parse_uint(&line, &number))
                {
                    goto CLEANUP_FILE;
                }

                if (!consume(&line, cstrlen(" ")))
                {
                    goto CLEANUP_FILE;
                }

                if (consume_if(&line, "red"))
                {
                    num_reds = max(num_reds, number);
                }
                else if (consume_if(&line, "green"))
                {
                    num_greens = max(num_greens, number);
                }
                else if (consume_if(&line, "blue"))
                {
                    num_blues = max(num_blues, number);
                }
                else
                {
                    goto CLEANUP_FILE;
                }

                consume_if(&line, ", ");
            }

            consume_if(&line, "; ");
        }

        if ((num_reds <= 12) && (num_greens <= 13) && (num_blues <= 14))
        {
            result.solve1 += game_id;
        }

        ULONGLONG power = num_reds * num_greens * num_blues;

        result.solve2 += power;
    }

CLEANUP_FILE:
    HeapFree(GetProcessHeap(), 0, file_data);
    file_data = NULL;

COMPLETE:
    return result;
}
