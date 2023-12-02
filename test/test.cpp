#include "pch.h"
#include "CppUnitTest.h"

#include "day_01/src/solve.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define PATH_PREFIX L"..\\..\\..\\..\\"

namespace test
{
	TEST_CLASS(test)
	{
	public:
		TEST_METHOD(Test01_Solve1)
		{
			Result01 result = solve01(PATH_PREFIX L"day_01\\res\\input.txt");
			Assert::AreEqual((int)result.solve1, (int)53651);
		}

		TEST_METHOD(Test01_Solve2)
		{
			Result01 result = solve01(PATH_PREFIX L"day_01\\res\\input.txt");
			Assert::AreEqual((int)result.solve2, (int)53894);
		}
	};
}
