#include "pch.h"
#include "CppUnitTest.h"

#include "day_01/src/solve.h"
#include "day_02/src/solve.h"
#include "day_03/src/solve.h"
#include "day_04/src/solve.h"
#include "day_05/src/solve.h"
#include "day_06/src/solve.h"
#include "day_07/src/solve.h"
#include "day_08/src/solve.h"
#include "day_09/src/solve.h"
#include "day_10/src/solve.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define PATH_PREFIX L"..\\..\\..\\..\\"

namespace AdventOfCode
{
    TEST_CLASS(Day01)
    {
    public:
        TEST_METHOD(Solve1)
        {
            Result01 result = solve01(PATH_PREFIX L"day_01\\res\\input.txt");
            Assert::AreEqual((int)53651, (int)result.solve1);
        }

        TEST_METHOD(Solve2)
        {
            Result01 result = solve01(PATH_PREFIX L"day_01\\res\\input.txt");
            Assert::AreEqual((int)53894, (int)result.solve2);
        }
    };

    TEST_CLASS(Day02)
    {
    public:
        TEST_METHOD(Solve1)
        {
            Result02 result = solve02(PATH_PREFIX L"day_02\\res\\input.txt");
            Assert::AreEqual((int)1867, (int)result.solve1);
        }

        TEST_METHOD(Solve2)
        {
            Result02 result = solve02(PATH_PREFIX L"day_02\\res\\input.txt");
            Assert::AreEqual((int)84538, (int)result.solve2);
        }
    };

    TEST_CLASS(Day03)
    {
    public:
        TEST_METHOD(Solve1)
        {
            Result03 result = solve03(PATH_PREFIX L"day_03\\res\\input.txt");
            Assert::AreEqual((int)556367, (int)result.solve1);
        }

        TEST_METHOD(Solve2)
        {
            Result03 result = solve03(PATH_PREFIX L"day_03\\res\\input.txt");
            Assert::AreEqual((int)89471771, (int)result.solve2);
        }
    };

    TEST_CLASS(Day04)
    {
    public:
        TEST_METHOD(Solve1)
        {
            Result04 result = solve04(PATH_PREFIX L"day_04\\res\\input.txt");
            Assert::AreEqual((int)25010, (int)result.solve1);
        }

        TEST_METHOD(Solve2)
        {
            Result04 result = solve04(PATH_PREFIX L"day_04\\res\\input.txt");
            Assert::AreEqual((int)9924412, (int)result.solve2);
        }
    };

    TEST_CLASS(Day05)
    {
    public:
        TEST_METHOD(Solve1)
        {
            Result05 result = solve05(PATH_PREFIX L"day_05\\res\\input.txt");
            Assert::AreEqual((int)318728750, (int)result.solve1);
        }

        TEST_METHOD(Solve2)
        {
            Result05 result = solve05(PATH_PREFIX L"day_05\\res\\input.txt");
            Assert::AreEqual((int)37384986, (int)result.solve2);
        }
    };

    TEST_CLASS(Day06)
    {
    public:
        TEST_METHOD(Solve1)
        {
            Result06 result = solve06(PATH_PREFIX L"day_06\\res\\input.txt");
            Assert::AreEqual((int)741000, (int)result.solve1);
        }

        TEST_METHOD(Solve2)
        {
            Result06 result = solve06(PATH_PREFIX L"day_06\\res\\input.txt");
            Assert::AreEqual((int)38220708, (int)result.solve2);
        }
    };

    TEST_CLASS(Day07)
    {
    public:
        TEST_METHOD(Solve1)
        {
            Result07 result = solve07(PATH_PREFIX L"day_07\\res\\input.txt");
            Assert::AreEqual((int)246409899, (int)result.solve1);
        }

        TEST_METHOD(Solve2)
        {
            Result07 result = solve07(PATH_PREFIX L"day_07\\res\\input.txt");
            Assert::AreEqual((int)244848487, (int)result.solve2);
        }
    };

    TEST_CLASS(Day08)
    {
    public:
        TEST_METHOD(Solve1)
        {
            Result08 result = solve08(PATH_PREFIX L"day_08\\res\\input.txt");
            Assert::AreEqual((int)18673, (int)result.solve1);
        }

        TEST_METHOD(Solve2)
        {
            Result08 result = solve08(PATH_PREFIX L"day_08\\res\\input.txt");
            Assert::AreEqual((int)0, (int)result.solve2);
        }
    };

    TEST_CLASS(Day09)
    {
    public:
        TEST_METHOD(Solve1)
        {
            Result09 result = solve09(PATH_PREFIX L"day_09\\res\\input.txt");
            Assert::AreEqual((int)1637452029, (int)result.solve1);
        }

        TEST_METHOD(Solve2)
        {
            Result09 result = solve09(PATH_PREFIX L"day_09\\res\\input.txt");
            Assert::AreEqual((int)908, (int)result.solve2);
        }
    };

    TEST_CLASS(Day10)
    {
    public:
        TEST_METHOD(Solve1)
        {
            Result10 result = solve10(PATH_PREFIX L"day_10\\res\\input.txt");
            Assert::AreEqual((int)7030, (int)result.solve1);
        }

        TEST_METHOD(Solve2)
        {
            Result10 result = solve10(PATH_PREFIX L"day_10\\res\\input.txt");
            Assert::AreEqual((int)285, (int)result.solve2);
        }
    };
}
