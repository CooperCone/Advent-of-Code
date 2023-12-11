#include "solve.h"

#include "str.h"
#include "io.h"

#define SKIP_SOLVE_2

#define NUM_LETTERS 26
#define NUM_NODES (NUM_LETTERS * NUM_LETTERS * NUM_LETTERS)

typedef struct
{
    SSIZE_T left;
    SSIZE_T right;
} Neighbors;

static Neighbors neighbors[NUM_NODES] = {0};

SIZE_T cur_nodes[NUM_LETTERS * NUM_LETTERS] = {0};
SIZE_T num_nodes = 0;

static SSIZE_T calculate_node_id(String node);

static SIZE_T get_next_node(SIZE_T cur_node, CHAR dir);

Result08 solve08(LPCWSTR filename)
{
    Result08 result = {0};

    SIZE_T file_size = 0;

    PBYTE file_data = read_file(filename, &file_size);

    if (file_data == NULL)
    {
        goto COMPLETE;
    }

    for (SIZE_T i = 0; i < NUM_NODES; i++)
    {
        neighbors[i] = (Neighbors){ -1, -1 } ;
    }

    String file_str =
    {
        .str = file_data,
        .size = file_size
    };

    String directions = tokenize(&file_str, "\r\n");

    tokenize(&file_str, "\r\n");

    for (;;)
    {
        String line = tokenize(&file_str, "\r\n");

        if (line.size == 0)
        {
            break;
        }

        String node = line;
        node.size = 3;

        SSIZE_T node_id = calculate_node_id(node);

        if (node_id == -1)
        {
            goto CLEANUP_FILE;
        }

        if (!consume(&line, 3))
        {
            goto CLEANUP_FILE;
        }

        if (!consume_if(&line, " = ("))
        {
            goto CLEANUP_FILE;
        }

        String left_node = line;
        left_node.size = 3;

        SSIZE_T left_id = calculate_node_id(left_node);

        if (left_id == -1)
        {
            goto CLEANUP_FILE;
        }

        if (!consume(&line, 3))
        {
            goto CLEANUP_FILE;
        }

        if (!consume_if(&line, ", "))
        {
            goto CLEANUP_FILE;
        }

        String right_node = line;
        right_node.size = 3;

        SSIZE_T right_id = calculate_node_id(right_node);

        if (right_id == -1)
        {
            goto CLEANUP_FILE;
        }

        neighbors[node_id] = (Neighbors)
        {
            .left = (ULONGLONG)left_id,
            .right = (ULONGLONG)right_id
        };
    }

    SIZE_T cur_node = 0;
    SIZE_T cur_direction = 0;

    while (cur_node != NUM_NODES - 1)
    {
       result.solve1++;

       cur_node = get_next_node(cur_node, directions.str[cur_direction]);
       cur_direction = (cur_direction + 1) % directions.size;
    }

    String s = { "AAA", 3};
    SSIZE_T min_a = calculate_node_id(s);

    s = (String){ "ZZA", 3 };
    SSIZE_T max_a = calculate_node_id(s);

    s = (String){ "AAZ", 3 };
    SSIZE_T min_z = calculate_node_id(s);

    s = (String){ "ZZZ", 3 };
    SSIZE_T max_z = calculate_node_id(s);

    if ((min_a == -1) || (max_a == -1) || (min_z == -1) || (max_z == -1))
    {
        goto CLEANUP_FILE;
    }

    for (SIZE_T i = min_a; i < (SIZE_T)max_a + 1; i++)
    {
        if ((neighbors[i].left != -1) && (neighbors[i].right != -1))
        {
            cur_nodes[num_nodes] = i;
            num_nodes++;
        }
    }

    cur_direction = 0;

#ifndef SKIP_SOLVE_2
    for (;;)
    {
        result.solve2++;

        BOOL found_all_zs = TRUE;

        for (SIZE_T i = 0; i < num_nodes; i++)
        {
            SIZE_T next = get_next_node(cur_nodes[i],
                directions.str[cur_direction]);

            if ((next < (SIZE_T)min_z) || (next > (SIZE_T)max_z))
            {
                found_all_zs = FALSE;
            }

            cur_nodes[i] = next;
        }

        if (found_all_zs)
        {
            break;
        }

        cur_direction = (cur_direction + 1) % directions.size;
    }
#endif

CLEANUP_FILE:
    HeapFree(GetProcessHeap(), 0, file_data);
    file_data = NULL;

COMPLETE:
    return result;
}

static SSIZE_T calculate_node_id(String node)
{
    if (node.size != 3)
    {
        return -1;
    }

    ULONGLONG right = node.str[2] - 'A';
    ULONGLONG middle = node.str[1] - 'A';
    ULONGLONG left = node.str[0] - 'A';

    return right * NUM_LETTERS * NUM_LETTERS +
        middle * NUM_LETTERS +
        left;
}

static SIZE_T get_next_node(SIZE_T cur_node, CHAR dir)
{
    SIZE_T next_node = 0;

    if (dir == 'L')
    {
        next_node = neighbors[cur_node].left;
    }
    else
    {
        next_node = neighbors[cur_node].right;
    }

    return next_node;
}


