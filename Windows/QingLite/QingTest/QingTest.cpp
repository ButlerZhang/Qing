// QingTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\..\Qing\HeaderFiles\RandomGenerator.h"
#include <Windows.h>
#include <iostream>
#include <string>



void NormalDistribution()
{
    std::default_random_engine e;           //生成随机整数
    std::normal_distribution<> n(4, 1.5);   //均值4，标准差1.5
    std::vector<unsigned> vals(9);          //9个元素均为0

    for (size_t i = 0; i != 200; i++)
    {
        unsigned v = lround(n(e));          //舍入到最接近的整数
        if (v < vals.size())
        {
            ++vals[v];
        }
    }

    for (size_t j = 0; j != vals.size(); j++)
    {
        std::cout << j << ": " << std::string(vals[j], '*') << std::endl;
    }
}


int main()
{
    NormalDistribution();
    std::cout << std::endl;
    system("pause");
    return 0;
}
