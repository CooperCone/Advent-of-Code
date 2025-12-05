from argparse import ArgumentParser
from pathlib import Path

def get_input(file_name):
    with open(Path(__file__).parent / file_name) as f:
        lines = f.readlines()
        return [[x for x in line.strip()] for line in lines]


def print_grid(grid):
    for row in grid:
        print("".join(row))


def add_sentinels(data):
    for row in data:
        row.insert(0, ".")
        row.append(".")

    empty_row = ["." for x in range(len(data[0]))]

    data.insert(0, empty_row)
    data.append(empty_row)

    return data


def get_accessible_positions(data):
    positions = []

    for y in range(1, len(data) - 1):
        for x in range(1, len(data[0]) - 1):
            if data[y][x] != "@":
                continue

            num_rolls = 0

            for i in [-1, 0, 1]:
                for ii in range(-1, 2):
                    if i == 0 and ii == 0:
                        continue

                    if data[y + i][x + ii] == "@":
                        num_rolls += 1

            if num_rolls < 4:
                positions.append((x, y))

    return positions


def puzzle1(is_test):
    filename = "test.txt" if is_test else "1.txt"
    puzzle_input = get_input(filename)

    puzzle_input = add_sentinels(puzzle_input)

    num_accessible = len(get_accessible_positions(puzzle_input))

    print(f"Accessible Rolls: {num_accessible}")


def puzzle2(is_test):
    filename = "test.txt" if is_test else "1.txt"
    puzzle_input = get_input(filename)

    puzzle_input = add_sentinels(puzzle_input)

    total_accessible = 0

    while True:
        accessible_positions = get_accessible_positions(puzzle_input)

        if len(accessible_positions) == 0:
            break

        total_accessible += len(accessible_positions)

        for x, y in accessible_positions:
            puzzle_input[y][x] = "."

    print(f"Total Accessible Rolls: {total_accessible}")



def day04():
    parser = ArgumentParser()
    parser.add_argument("puzzle", type=int, choices=[1, 2])
    parser.add_argument("--test", default=False, action="store_true")
    args = parser.parse_args()

    if args.puzzle == 1:
        puzzle1(args.test)
    else:
        puzzle2(args.test)