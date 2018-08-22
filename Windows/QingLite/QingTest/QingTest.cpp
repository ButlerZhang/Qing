// QingTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\..\Qing\Example\DesignPatterns\Header\AbstractFactory.h"
#include "..\..\Qing\Example\DesignPatterns\Header\Adapter.h"
#include <Windows.h>
#include <iostream>



int main()
{
    QingTemplate::AbstractFactoryTest();
    std::cout << std::endl;
    QingTemplate::TestAdapter();

    std::cout << std::endl;
    system("pause");
    return 0;
}

