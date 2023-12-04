#include "solve.h"

#include "str.h"
#include "io.h"

static PULONGLONG card_copies = NULL;
static SIZE_T num_cards = 0;

static BOOL add_card_copy(ULONGLONG card_number, ULONGLONG num_copies);

static BOOL parse_scratch_number(String *number_list, PULONGLONG out_number);

Result04 solve04(LPCWSTR filename)
{
    Result04 result = {0};

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

        if (!consume_if(&line, "Card "))
        {
            goto CLEANUP_COPIES;
        }

        ULONGLONG card_number = 0;

        if (!parse_scratch_number(&line, &card_number))
        {
            goto CLEANUP_COPIES;
        }

        String winning = string_find(line, ':');

        if (winning.size == 0)
        {
            goto CLEANUP_COPIES;
        }

        if (!consume(&winning, 2))
        {
            goto CLEANUP_COPIES;
        }

        String numbers = string_find(winning, '|');

        if (numbers.size == 0)
        {
            goto CLEANUP_COPIES;
        }

        winning.size -= numbers.size + 1;

        if (!consume(&numbers, 2))
        {
            goto CLEANUP_COPIES;
        }

        SIZE_T num_winning_numbers = 0;

        while (winning.size > 0)
        {
            ULONGLONG winning_number = 0;

            if (!parse_scratch_number(&winning, &winning_number))
            {
                goto CLEANUP_COPIES;
            }

            String numbers_copy = numbers;

            while (numbers_copy.size > 0)
            {
                ULONGLONG our_number = 0;

                if (!parse_scratch_number(&numbers_copy, &our_number))
                {
                    goto CLEANUP_COPIES;
                }

                if (our_number == winning_number)
                {
                    num_winning_numbers++;
                }
            }
        }

        if (!add_card_copy(card_number, 1))
        {
            goto CLEANUP_COPIES;
        }

        ULONGLONG copies = card_copies[card_number];

        for (SIZE_T i = 0; i < num_winning_numbers; i++)
        {
            SIZE_T idx = card_number + i + 1;

            if (!add_card_copy(idx, copies))
            {
                goto CLEANUP_COPIES;
            }
        }

        result.solve1 += (1ULL << num_winning_numbers) >> 1;
        result.solve2 += card_copies[card_number];
    }

CLEANUP_COPIES:
    HeapFree(GetProcessHeap(), 0, card_copies);
    card_copies = NULL;
    num_cards = 0;

    HeapFree(GetProcessHeap(), 0, file_data);
    file_data = NULL;

COMPLETE:
    return result;
}

static BOOL add_card_copy(ULONGLONG card_number, ULONGLONG num_copies)
{
    if ((card_number == 0) || (num_copies == 0))
    {
        return FALSE;
    }

    BOOL result = FALSE;

    if (card_number >= num_cards)
    {
        if (card_copies == NULL)
        {
            num_cards = 8;
            card_copies = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                num_cards * sizeof *card_copies);

            if (card_copies == NULL)
            {
                goto COMPLETE;
            }
        }
        else
        {
            SIZE_T new_num_cards = num_cards * 2;

            PULONGLONG new_cards = HeapReAlloc(GetProcessHeap(),
                HEAP_ZERO_MEMORY, card_copies,
                new_num_cards * sizeof *card_copies);

            if (new_cards == NULL)
            {
                goto COMPLETE;
            }

            card_copies = new_cards;
            num_cards = new_num_cards;
        }
    }

    card_copies[card_number] += num_copies;
    result = TRUE;

COMPLETE:
    return result;
}

static BOOL parse_scratch_number(String *number_list, PULONGLONG out_number)
{
    if ((number_list == NULL) || (number_list->str == NULL) ||
        (number_list->size == 0) || (out_number == NULL))
    {
        return FALSE;
    }

    while (number_list->str[0] == ' ')
    {
        if (!consume(number_list, 1))
        {
            return FALSE;
        }
    }

    BOOL result = parse_uint(number_list, out_number);

    if (result)
    {
        if (number_list->size > 0)
        {
            // Don't really care if this fails
            (void)consume_if(number_list, " ");
        }
    }

    return result;
}
