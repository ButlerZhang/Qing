// QingTest.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <numeric>

#include "..\..\Qing\HeaderFiles\QingLog.h"
#include "..\..\Qing\HeaderFiles\Utility.h"
#include "..\..\Qing\HeaderFiles\RandomGenerator.h"



double StorageStringAsDouble(const std::wstring &csInput, double dDefault)
{
    double dOutput = 0.0f;
    long long llTemp = 0;
    if (_stscanf(csInput.c_str(), L"%lld", &llTemp) > 0)
        dOutput = *(double*)&llTemp;
    else
        dOutput = dDefault;
    return dOutput;
}

int main()
{
    std::vector<std::wstring> ProbWinVector = {
        L"4591841168814906923",
        L"4594145047714251128",
        L"4593416554697928511",
        L"4590549355288985654",
        L"4594504687346238412",
        L"4596811805810895374",
        L"4586650350148589186",
        L"4584763574794864878",
        L"4588074880792361392",
        L"4589490066574993195"
    };

    for (std::vector<std::wstring>::size_type Index = 0; Index < ProbWinVector.size(); Index++)
    {
        std::wcout << ProbWinVector[Index] << "\t" << StorageStringAsDouble(ProbWinVector[Index], 0.0) << std::endl;
    }

    std::cout << std::endl;
    system("pause");
    return 0;
}
