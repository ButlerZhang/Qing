#include "AlgorithmTest.h"
#include "src/001_MaxSubSum.h"
#include "src/002_BinarySearch.h"
#include "src/003_CommonMath.h"
#include "src/004_VectorLite.h"

#include <iostream>
#include <string>



class TestOutputMessage
{
public:
    TestOutputMessage(const std::string &FunctionName)
    {
        std::cout << "=====Test " << FunctionName << ":=====" << std::endl << std::endl;
    }

    ~TestOutputMessage()
    {
        std::cout << std::endl << "=====Test Stop.=====" << std::endl << std::endl;
    }
};



void TestMaxSubSum()
{
    TestOutputMessage Test("Max Sub Sum");

    MaxSubSum TestObject;
    std::vector<int> Array = { -2, 11, -4, 13, -5, -2 };

    std::cout << TestObject.MaxSubSumTripleCycle(Array) << std::endl;
    std::cout << TestObject.MaxSubSumTwoCycle(Array) << std::endl;
    std::cout << TestObject.MaxSubSumRecursive(Array) << std::endl;
    std::cout << TestObject.MaxSubSumSingleCycle(Array) << std::endl;
}

void TestBinarySearch()
{
    TestOutputMessage Test("Binary Search");

    BinarySearch TestObject;
    std::vector<int> Array = { 1,2,3,4,5,6,7,8,9,10 };
    std::cout << TestObject.GetBinarySearch(Array, 11) << std::endl;
    std::cout << TestObject.GetBinarySearch(Array, 1) << std::endl;
    std::cout << TestObject.GetBinarySearch(Array, 9) << std::endl;
    std::cout << TestObject.GetBinarySearch(Array, 10) << std::endl;
}

void TestGreatestCommonDivisor()
{
    TestOutputMessage Test("Greatest Common Divisor");

    CommonMath TestObject;
    std::cout << TestObject.GreatestCommonDivisor(50, 15) << std::endl;
    std::cout << TestObject.GreatestCommonDivisor(15, 50) << std::endl;
}

void TestPowRecursive()
{
    TestOutputMessage Test("Pow Recursive");

    CommonMath TestObject;
    std::cout << TestObject.PowRecursive(2, 10) << std::endl;
    std::cout << TestObject.PowRecursive(3, 2) << std::endl;
}

void TestVectorLite()
{
    TestOutputMessage Test("Vector Lite");
    VectorLite<int> TestObject(10);

    for(int count = 0; count < 10; count++)
    {
        TestObject.push_back(count);
    }

    for(int Index = 0; Index < TestObject.size(); Index++)
    {
        std::cout << TestObject[Index] << std::endl;
    }
}
