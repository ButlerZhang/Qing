// QingTest.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "..\..\Qing\HeaderFiles\CommonFunction.h"
#include "..\..\Qing\HeaderFiles\BoostLog.h"
#include <iostream>



int main()
{
    try
    {
        Qing::BoostLog::DefaultInit();
        Qing::BoostLog::WriteError(L"Chinese");
        Qing::BoostLog::WriteError(L"ÖÐ¹úÈË");
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
    }

    system("pause");
    return 0;
}
