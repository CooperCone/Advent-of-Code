import argparse

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("filename")
    return parser.parse_args()


def main():
    args = parse_args()

    with open(args.filename, "r") as f:
        all_data = f.read()

    lines = all_data.split("\n")
    parsed_data = [[int(x) for x in line.split()] for line in lines]

    column1 = [x[0] for x in parsed_data]
    column2 = [x[1] for x in parsed_data]

    pairs = zip(sorted(column1), sorted(column2))

    diffs = [abs(x[0] - x[1]) for x in pairs]

    print(f"Solve 1: {sum(diffs)}")

    counts = [len([x for x in column2 if x == num]) for num in column1 ]

    nums = [num * count  for num, count in zip(column1, counts)]

    print(f"Solve 2: {sum(nums)}")

if __name__ == '__main__':
    main()
