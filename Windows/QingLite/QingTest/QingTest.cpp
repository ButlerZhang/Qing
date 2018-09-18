// QingTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <string>

#include "..\..\Qing\HeaderFiles\QingLog.h"



int main()
{
    Qing::QingLog mylog;
    mylog.TestBoostLog();

    std::cout << std::endl;
    system("pause");
    return 0;
}
