#include "solve.h"

#include "str.h"
#include "io.h"

#define NUM_CARDS 13
#define HAND_SIZE 5

typedef ULONGLONG (*ValueFunc)(Hand);

typedef struct
{
    String hand;
    ULONGLONG bid;
} Hand;

static CHAR solve_1_card_values[NUM_CARDS] =
    { '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A' };

static CHAR solve_2_card_values[NUM_CARDS] =
    { 'J', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'Q', 'K', 'A' };

static Hand *hands = NULL;
static SIZE_T num_hands = 0;

static BOOL append_hand(Hand hand);

static ULONGLONG solve_1_calculate_hand_type(Hand hand);

static ULONGLONG solve_1_calculate_hand_value(Hand hand);

static ULONGLONG solve_2_calculate_hand_type(Hand hand);

static ULONGLONG solve_2_calculate_hand_value(Hand hand);

static ULONGLONG calculate_hand_type_value(ULONGLONG numbers[NUM_CARDS]);

static BOOL check_hand(ULONGLONG numbers[NUM_CARDS], ULONGLONG number1,
    ULONGLONG number2);

static ULONGLONG get_card_value(CHAR value, CHAR card_values[NUM_CARDS]);

static void sort_hands(ValueFunc get_type, ValueFunc get_value);

Result07 solve07(LPCWSTR filename)
{
    Result07 result = {0};

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

        String hand = line;
        hand.size = HAND_SIZE;

        if (!consume(&line, HAND_SIZE))
        {
            goto CLEANUP_HANDS;
        }

        if (!consume_if(&line, " "))
        {
            goto CLEANUP_HANDS;
        }

        ULONGLONG bid = 0;

        if (!parse_uint(&line, &bid))
        {
            goto CLEANUP_HANDS;
        }

        Hand hand_res =
        {
            .hand = hand,
            .bid = bid,
        };

        if (!append_hand(hand_res))
        {
            goto CLEANUP_HANDS;
        }
    }

    sort_hands(solve_1_calculate_hand_type, solve_1_calculate_hand_value);

    for (SIZE_T i = 0; i < num_hands; i++)
    {
        result.solve1 += (i + 1) * hands[i].bid;
    }

    sort_hands(solve_2_calculate_hand_type, solve_2_calculate_hand_value);

    for (SIZE_T i = 0; i < num_hands; i++)
    {
        result.solve2 += (i + 1) * hands[i].bid;
    }

CLEANUP_HANDS:
    HeapFree(GetProcessHeap(), 0, hands);
    hands = NULL;
    num_hands = 0;

    HeapFree(GetProcessHeap(), 0, file_data);
    file_data = NULL;

COMPLETE:
    return result;
}

static BOOL append_hand(Hand hand)
{
    BOOL result = FALSE;

    num_hands++;

    if (num_hands == 1)
    {
        hands = HeapAlloc(GetProcessHeap(), 0, num_hands * sizeof *hands);

        if (hands == NULL)
        {
            goto COMPLETE;
        }
    }
    else
    {
        Hand *new_hands = HeapReAlloc(GetProcessHeap(), 0, hands,
            num_hands * sizeof *hands);

        if (new_hands == NULL)
        {
            goto COMPLETE;
        }

        hands = new_hands;
    }

    hands[num_hands - 1] = hand;

    result = TRUE;

COMPLETE:
    return result;
}

static ULONGLONG solve_1_calculate_hand_type(Hand hand)
{
    ULONGLONG numbers[NUM_CARDS] = {0};

    for (SIZE_T i = 0; i < HAND_SIZE; i++)
    {
        numbers[get_card_value(hand.hand.str[i], solve_1_card_values)]++;
    }

    return calculate_hand_type_value(numbers);
}

static ULONGLONG solve_1_calculate_hand_value(Hand hand)
{
    ULONGLONG places_values[] =
    {
        NUM_CARDS * NUM_CARDS * NUM_CARDS * NUM_CARDS,
        NUM_CARDS * NUM_CARDS * NUM_CARDS,
        NUM_CARDS * NUM_CARDS,
        NUM_CARDS,
        1,
    };

    ULONGLONG value = 0;

    for (SIZE_T i = 0; i < HAND_SIZE; i++)
    {
        value += get_card_value(hand.hand.str[i], solve_1_card_values) *
            places_values[i];
    }

    return value;
}

static ULONGLONG solve_2_calculate_hand_type(Hand hand)
{
    ULONGLONG numbers[NUM_CARDS] = {0};

    for (SIZE_T i = 0; i < HAND_SIZE; i++)
    {
        numbers[get_card_value(hand.hand.str[i], solve_2_card_values)]++;
    }

    ULONGLONG jacks = numbers[0];

    numbers[0] = 0;

    ULONGLONG max_idx = 0;
    ULONGLONG max_value = 0;

    for (SIZE_T i = 0; i < NUM_CARDS; i++)
    {
        if (numbers[i] > max_value)
        {
            max_idx = i;
            max_value = numbers[i];
        }
    }

    numbers[max_idx] += jacks;

    return calculate_hand_type_value(numbers);
}

static ULONGLONG solve_2_calculate_hand_value(Hand hand)
{
    ULONGLONG places_values[] =
    {
        NUM_CARDS * NUM_CARDS * NUM_CARDS * NUM_CARDS,
        NUM_CARDS * NUM_CARDS * NUM_CARDS,
        NUM_CARDS * NUM_CARDS,
        NUM_CARDS,
        1,
    };

    ULONGLONG value = 0;

    for (SIZE_T i = 0; i < HAND_SIZE; i++)
    {
        value += get_card_value(hand.hand.str[i], solve_2_card_values) *
            places_values[i];
    }

    return value;
}

static ULONGLONG calculate_hand_type_value(ULONGLONG numbers[NUM_CARDS])
{
    if (check_hand(numbers, 5, 0))
    {
        return 6;
    }

    if (check_hand(numbers, 4, 0))
    {
        return 5;
    }

    if (check_hand(numbers, 3, 2))
    {
        return 4;
    }

    if (check_hand(numbers, 3, 0))
    {
        return 3;
    }

    if (check_hand(numbers, 2, 2))
    {
        return 2;
    }

    if (check_hand(numbers, 2, 0))
    {
        return 1;
    }

    return 0;
}

static BOOL check_hand(ULONGLONG numbers[NUM_CARDS], ULONGLONG number1,
    ULONGLONG number2)
{
    BOOL found_1 = FALSE;
    BOOL found_2 = FALSE;

    for (SIZE_T i = 0; i < NUM_CARDS; i++)
    {
        if ((!found_1) && (numbers[i] == number1))
        {
            found_1 = TRUE;
        }
        else if ((!found_2) && (numbers[i] == number2))
        {
            found_2 = TRUE;
        }
    }

    return found_1 && found_2;
}

static ULONGLONG get_card_value(CHAR value, CHAR card_values[NUM_CARDS])
{
    ULONGLONG card_value = 0;

    for (SIZE_T i = 0; i < NUM_CARDS; i++)
    {
        if (card_values[i] == value)
        {
            card_value = i;
            break;
        }
    }

    return card_value;
}

static void sort_hands(ValueFunc get_type, ValueFunc get_value)
{
    for (SIZE_T i = 0; i < num_hands; i++)
    {
        for (SIZE_T j = i + 1; j < num_hands; j++)
        {
            Hand hand1 = hands[i];
            Hand hand2 = hands[j];

            ULONGLONG type_1 = get_type(hand1);
            ULONGLONG type_2 = get_type(hand2);

            ULONGLONG value_1 = get_value(hand1);
            ULONGLONG value_2 = get_value(hand2);

            if ((type_1 > type_2) ||
                ((type_1 == type_2) && (value_1 > value_2)))
            {
                hands[i] = hand2;
                hands[j] = hand1;
            }
        }
    }
}
