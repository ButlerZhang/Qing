#include <iostream>
#include "UNIX/UNIXTest.h"
#include "../../Common/Algorithm/AlgorithmTest.h"
#include "../../Common/Tool/Common.h"



void TestUNIX();
void TestAlgorithm();

int main(int argc, char *argv[])
{
    std::cout << "Tool test" << std::endl;
    if (IsBigEndian())
        std::cout << "big endian" << std::endl;
    else std::cout << "little endian" << std::endl;

    //TestUNIX();
    //TestAlgorithm();

    std::cout << std::endl;
    return 0;
}

void TestAlgorithm()
{
    //TestMaxSubSum();
    //TestBinarySearch();
    //TestGreatestCommonDivisor();
    //TestPowRecursive();
    TestVectorLite();
}

void TestUNIX()
{
    TestShowProcessInformation();

    //TestListFileName();
    //TestReadToWriteNotBufferIO();
    //TestReadToWriteStandardIO();	
    //TestExecuteCommand();
    //TestHowToUseErrno();
}