#include <windows.h>

#include "solve.h"
#include "str.h"
#include "io.h"

static LPCWSTR example = L"res\\example.txt";
static LPCWSTR input = L"res\\input.txt";

int mainCRTStartup()
{
    Result15 result = solve15(input);

    print_solve_number(L"Solve 1:", result.solve1);
    print_solve_number(L"Solve 2:", result.solve2);

    return 0;
}
