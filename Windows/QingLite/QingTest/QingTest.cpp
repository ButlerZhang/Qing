// QingTest.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "..\..\Qing\SourceCode\System\SystemTime.h"
#include "..\..\Qing\SourceCode\Example\Windows\WindowsCoreProgramming.h"
#include <iostream>


int main()
{
    //Qing::ElapsedTime TimeWatch;

    //Sleep(10000);
    //__int64 Elapsed = TimeWatch.Now();
    //std::cout << "Elapsed = " << Elapsed << " ms" << std::endl;

    //TimeWatch.Start();
    //Sleep(12345);
    //Elapsed = TimeWatch.Now();
    //std::cout << "Elapsed = " << Elapsed << " ms" << std::endl;

    AlignExample();

    system("pause");
    return 0;
}
