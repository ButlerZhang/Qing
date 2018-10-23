// QingTest.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "..\..\Qing\HeaderFiles\CommonFunction.h"
#include <iostream>



int main()
{
    std::cout << Qing::GetSHA1("Qing") << std::endl;
    std::wcout << Qing::GetSHA1(L"Qing") << std::endl;
    system("pause");
    return 0;
}
