#include <windows.h>

#include "solve.h"
#include "str.h"
#include "io.h"

static LPCWSTR example1 = L"res\\example1.txt";
static LPCWSTR example2 = L"res\\example2.txt";
static LPCWSTR input = L"res\\input.txt";

int mainCRTStartup()
{
    Result08 result = solve08(input);

    print_solve_number(L"Solve 1:", result.solve1);
    print_solve_number(L"Solve 2:", result.solve2);

    return 0;
}
