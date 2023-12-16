#include "solve.h"

#include "str.h"
#include "io.h"

#define NUM_BOXES 256

typedef struct Lens
{
    struct Lens *next;
    String label;
    SIZE_T focal_length;
} Lens;

static Lens *boxes[NUM_BOXES];

static BOOL hash(String str, ULONGLONG *out_hash);

static Lens *remove_lens(Lens *lenses, String label);

static Lens *append_lens(Lens *lenses, String label, SIZE_T focal_length);

Result15 solve15(LPCWSTR filename)
{
    Result15 result = {0};

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
        String line = tokenize(&file_str, ",");

        if (line.str == NULL)
        {
            break;
        }

        ULONGLONG hash_value = 0;

        if (!hash(line, &hash_value))
        {
            goto CLEANUP_FILE_DATA;
        }

        result.solve1 += hash_value;

        String label = { .str = line.str };

        while ((label.str[label.size] != '-') && (label.str[label.size] != '='))
        {
            label.size++;
        }

        ULONGLONG label_hash = 0;

        if (!hash(label, &label_hash))
        {
            goto CLEANUP_FILE_DATA;
        }

        CHAR op = label.str[label.size];

        if (op == '-')
        {
            boxes[label_hash] = remove_lens(boxes[label_hash], label);
        }
        else if (op == '=')
        {
            String number = string_find(line, '=');

            consume_if(&number, "=");

            ULONGLONG focal_length = 0;

            if (!parse_uint(&number, &focal_length))
            {
                goto CLEANUP_BOXES;
            }

            boxes[label_hash] = append_lens(boxes[label_hash], label,
                focal_length);
        }
        else
        {
            OutputDebugStringW(L"Shouldn't be here\n");
            break;
        }
    }

    for (SIZE_T i = 0; i < NUM_BOXES; i++)
    {
        SIZE_T box_num = 1;

        Lens *cur = boxes[i];

        while (cur != NULL)
        {
            ULONGLONG focusing_power = 1 + i;
            focusing_power *= box_num;
            focusing_power *= cur->focal_length;

            result.solve2 += focusing_power;

            box_num++;
            cur = cur->next;
        }
    }

CLEANUP_BOXES:
    for (SIZE_T i = 0; i < NUM_BOXES; i++)
    {
        while (boxes[i] != NULL)
        {
            boxes[i] = remove_lens(boxes[i], boxes[i]->label);
        }
    }

CLEANUP_FILE_DATA:
    HeapFree(GetProcessHeap(), 0, file_data);
    file_data = NULL;

COMPLETE:
    return result;
}

static BOOL hash(String str, ULONGLONG *out_hash)
{
    if ((str.str == NULL) || (str.size == 0) || (out_hash == NULL))
    {
        return FALSE;
    }

    ULONGLONG value = 0;

    for (SIZE_T i = 0; i < str.size; i++)
    {
        value += str.str[i];
        value *= 17;
        value &= 0xFF;
    }

    *out_hash = value;
    return TRUE;
}

static Lens *remove_lens(Lens *lenses, String label)
{
    if ((lenses == NULL) || (label.size == 0))
    {
        return lenses;
    }

    if (string_cmp(lenses->label, label))
    {
        Lens *next = lenses->next;

        *lenses = (Lens){0};

        HeapFree(GetProcessHeap(), 0, lenses);
        lenses = next;
    }
    else
    {
        lenses->next = remove_lens(lenses->next, label);
    }

    return lenses;
}

static Lens *append_lens(Lens *lenses, String label, SIZE_T focal_length)
{
    if (lenses != NULL)
    {
        if (string_cmp(lenses->label, label))
        {
            lenses->label = label;
            lenses->focal_length = focal_length;
        }
        else
        {
            lenses->next = append_lens(lenses->next, label, focal_length);
        }
    }
    else
    {
        Lens *new_lens = HeapAlloc(GetProcessHeap(), 0, sizeof *new_lens);

        if (new_lens == NULL)
        {
            goto COMPLETE;
        }

        new_lens->next = NULL;
        new_lens->label = label;
        new_lens->focal_length = focal_length;

        lenses = new_lens;
    }

COMPLETE:
    return lenses;
}
