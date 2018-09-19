// QingTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <numeric>

#include "..\..\Qing\HeaderFiles\QingLog.h"
#include "..\..\Qing\HeaderFiles\Utility.h"
#include "..\..\Qing\HeaderFiles\RandomGenerator.h"




int main()
{
    typedef unsigned long long NewType;
    //typedef __int128 NewType;

    std::vector<NewType> ProbWinVector = {
        4591841168814906923,
        4594145047714251128,
        4593416554697928511,
        4590549355288985654,
        4594504687346238412,
        4596811805810895374,
        4586650350148589186,
        4584763574794864878,
        4588074880792361392,
        4589490066574993195
    };

    for (auto Index = 0; Index < ProbWinVector.size(); Index++)
    {
        //std::cout << ProbWinVector[Index] << std::endl;
        std::cout << ProbWinVector[Index] << "\t" << static_cast<double>(ProbWinVector[Index]) << std::endl;
    }

    NewType InitValue = 0;
    NewType TotalProbWin = std::accumulate(ProbWinVector.begin(), ProbWinVector.end(), InitValue);
    std::cout << std::endl << TotalProbWin << "\t" << static_cast<double>(TotalProbWin)  << std::endl << std::endl;


    for (auto Index = 0; Index < ProbWinVector.size(); Index++)
    {
        double probwin = static_cast<double>(ProbWinVector[Index]) / static_cast<double>(TotalProbWin);
        std::cout << probwin << std::endl;
    }


    std::cout << std::endl;
    system("pause");
    return 0;
}
