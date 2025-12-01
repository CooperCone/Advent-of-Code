from argparse import ArgumentParser
from pathlib import Path

def get_input(file_name):
    with open(Path(__file__).parent / file_name) as f:
        all_lines = f.readlines()
        return [(line[0], int(line[1:])) for line in all_lines]


def puzzle1(is_test):
    filename = "test.txt" if is_test else "1.txt"
    puzzle_input = get_input(filename)

    password = 0
    current_value = 50

    for dir, value in puzzle_input:
        if dir == "R":
            current_value = (current_value + value) % 100
        else:
            current_value = (current_value - value) % 100

        if current_value == 0:
            password += 1

    print(f"Password: {password}")


def puzzle2(is_test):
    filename = "test.txt" if is_test else "1.txt"
    puzzle_input = get_input(filename)

    password = 0
    current_value = 50

    for dir, value in puzzle_input:
        if dir == "R":
            dir = 1
        else:
            dir = -1

        for _ in range(value):
            current_value += dir

            if current_value == 100:
                current_value = 0
            elif current_value == -1:
                current_value = 99

            if current_value == 0:
                password += 1

    print(f"Password: {password}")


def day01():
    parser = ArgumentParser()
    parser.add_argument("puzzle", type=int, choices=[1, 2])
    parser.add_argument("--test", default=False, action="store_true")
    args = parser.parse_args()

    if args.puzzle == 1:
        puzzle1(args.test)
    else:
        puzzle2(args.test)
