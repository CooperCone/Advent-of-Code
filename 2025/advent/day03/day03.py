from argparse import ArgumentParser
from pathlib import Path

def get_input(file_name):
    with open(Path(__file__).parent / file_name) as f:
        lines = f.readlines()
        return [[int(x) for x in line.strip()] for line in lines]


def find_max(joltages):
    idx = 0
    max_joltage = -1

    for i, joltage in enumerate(joltages):
        if joltage > max_joltage:
            idx = i
            max_joltage = joltage

    return idx, max_joltage


def puzzle1(is_test):
    filename = "test.txt" if is_test else "1.txt"
    puzzle_input = get_input(filename)

    total_joltages = 0

    for joltages in puzzle_input:
        idx, max_joltage = find_max(joltages[:-1])

        _, next_max_joltage = find_max(joltages[idx + 1:])

        total_joltages += max_joltage * 10 + next_max_joltage

    print(f"Total Joltage: {total_joltages}")


def puzzle2(is_test):
    filename = "test.txt" if is_test else "1.txt"
    puzzle_input = get_input(filename)

    total_joltages = 0

    for joltages in puzzle_input:
        total_joltage = 0

        prev_idx = 0

        for i in range(12):
            max_idx = -12 + i + 1
            if max_idx == 0:
                max_idx = len(joltages)
            idx, max_joltage = find_max(joltages[prev_idx:max_idx])

            total_joltage *= 10
            total_joltage += max_joltage
            prev_idx += idx + 1

        total_joltages += total_joltage

    print(f"Total Joltage: {total_joltages}")


def day03():
    parser = ArgumentParser()
    parser.add_argument("puzzle", type=int, choices=[1, 2])
    parser.add_argument("--test", default=False, action="store_true")
    args = parser.parse_args()

    if args.puzzle == 1:
        puzzle1(args.test)
    else:
        puzzle2(args.test)