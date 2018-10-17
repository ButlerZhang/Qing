// QingTest.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <numeric>
#include "UnitTest\TestDatabase.h"



int main()
{
    TestMySQLDatabase();
    system("pause");
    return 0;
}
