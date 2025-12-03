from argparse import ArgumentParser
from pathlib import Path

def get_input(file_name):
    with open(Path(__file__).parent / file_name) as f:
        line = f.readlines()[0]
        ranges = [x.split("-") for x in line.split(",")]
        return ranges


def puzzle1(is_test):
    filename = "test.txt" if is_test else "1.txt"
    puzzle_input = get_input(filename)

    num_repeating = 0

    for left, right in puzzle_input:
        for value in range(int(left), int(right) + 1):
            str_value = str(value)

            if len(str_value) % 2 != 0:
                continue

            if str_value[:len(str_value) // 2] == str_value[len(str_value) // 2:]:
                num_repeating += value

    print(f"Repeating: {num_repeating}")


def check_repeating(small, large):
    if len(large) % len(small) != 0:
        return False

    num_iters = len(large) // len(small)

    for i in range(num_iters):
        slice = large[i * len(small):(i + 1) * len(small)]

        if slice != small:
            return False

    return True


def puzzle2(is_test):
    filename = "test.txt" if is_test else "1.txt"
    puzzle_input = get_input(filename)

    num_repeating = 0

    for left, right in puzzle_input:
        for value in range(int(left), int(right) + 1):
            str_value = str(value)

            for i in range(1, len(str_value)):
                if check_repeating(str_value[:i], str_value[i:]):
                    num_repeating += value
                    break

    print(f"Repeating: {num_repeating}")


def day02():
    parser = ArgumentParser()
    parser.add_argument("puzzle", type=int, choices=[1, 2])
    parser.add_argument("--test", default=False, action="store_true")
    args = parser.parse_args()

    if args.puzzle == 1:
        puzzle1(args.test)
    else:
        puzzle2(args.test)