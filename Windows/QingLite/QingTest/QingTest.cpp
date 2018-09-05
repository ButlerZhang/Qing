// QingTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <string>

#include "..\..\Qing\Example\IOExample.h"



int main()
{
    QingTemplate::IOExample myIO;

    myIO.TestBoolapha();
    std::cout << std::endl;

    myIO.TestIntDecimal();
    std::cout << std::endl;

    myIO.TestShowbase();
    std::cout << std::endl;

    myIO.TestUppercase();
    std::cout << std::endl;

    myIO.TestPrecision();
    std::cout << std::endl;

    myIO.TestDoubleCounting();
    std::cout << std::endl;

    myIO.TestShowpoint();
    std::cout << std::endl;

    myIO.TestSpace();
    std::cout << std::endl;

    //myIO.TestNoSkipws();
    //std::cout << std::endl;

    //myIO.TestGetPut();
    //std::cout << std::endl;

    //myIO.TestEOF();
    //std::cout << std::endl;

    myIO.TestSeekTell();
    std::cout << std::endl;

    std::cout << std::endl;
    system("pause");
    return 0;
}
