#include "pch.h"
#include "CppUnitTest.h"

#include "day_01/src/solve.h"
#include "day_02/src/solve.h"
#include "day_03/src/solve.h"

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
}
