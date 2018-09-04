// QingTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\..\Qing\HeaderFiles\RandomAdapter.h"
#include <Windows.h>
#include <iostream>
#include <string>




int main()
{
    Qing::RandomAdapter ra;

    for (int i = 0; i < 10; i++)
    {
        std::cout << ra.GetRandom() << std::endl;
        std::cout << ra.GetGetRandom(0, 9) << std::endl;
    }

    std::cout << std::endl;
    system("pause");
    return 0;
}

