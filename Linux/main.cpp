#include <iostream>
#include "basic/unix/UNIXTest.h"
#include "basic/algorithm/AlgorithmTest.h"



void TestUNIX();
void TestAlgorithm();

int main(int argc, char *argv[])
{
	TestUNIX();
	TestAlgorithm();

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