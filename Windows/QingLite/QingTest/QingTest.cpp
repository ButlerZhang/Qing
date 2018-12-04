// QingTest.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "..\..\Qing\SourceCode\System\SystemShare.h"
#include <iostream>
#include <string>
#include <VersionHelpers.h>


int main()
{
    std::vector<std::wstring> EnvVector;
    Qing::SplitEnvironment(EnvVector);

    for (auto Index = 0; Index < EnvVector.size(); Index++)
    {
        std::wcout << EnvVector[Index] << std::endl;
    }

    if (IsWindowsXPOrGreater())
    {

    }

    system("pause");
    return 0;
}
