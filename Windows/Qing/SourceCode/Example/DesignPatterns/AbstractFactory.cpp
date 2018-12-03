#include "AbstractFactory.h"
#include <iostream>

QING_EXAMPLE_BEGIN



AbstractProductA1::AbstractProductA1()
{
    std::cout << "construction of AbstractProductA1." << std::endl;
}

AbstractProductA1::~AbstractProductA1()
{
    std::cout << "destruction of AbstractProductA1." << std::endl;
}

AbstractProductA2::AbstractProductA2()
{
    std::cout << "construction of AbstractProductA2." << std::endl;
}

AbstractProductA2::~AbstractProductA2()
{
    std::cout << "destruction of AbstractProductA2." << std::endl;
}



AbstractProductB1::AbstractProductB1()
{
    std::cout << "construction of AbstractProductB1." << std::endl;
}

AbstractProductB1::~AbstractProductB1()
{
    std::cout << "destruction of AbstractProductB1." << std::endl;
}

AbstractProductB2::AbstractProductB2()
{
    std::cout << "construction of AbstractProductB2." << std::endl;
}

AbstractProductB2::~AbstractProductB2()
{
    std::cout << "destruction of AbstractProductB2." << std::endl;
}



AbstractFactory1::AbstractFactory1()
{
    std::cout << "construction of AbstractFactory1." << std::endl;
}

AbstractFactory1::~AbstractFactory1()
{
    std::cout << "destruction of AbstractFactory1." << std::endl;
}

AbstractProductA* AbstractFactory1::GetAbstractProductA()
{
    return new AbstractProductA1();
}

AbstractProductB* AbstractFactory1::GetAbstractProductB()
{
    return new AbstractProductB1();
}



AbstractFactory2::AbstractFactory2()
{
    std::cout << "construction of AbstractFactory2." << std::endl;
}

AbstractFactory2::~AbstractFactory2()
{
    std::cout << "destruction of AbstractFactory2." << std::endl;
}

AbstractProductA* AbstractFactory2::GetAbstractProductA()
{
    return new AbstractProductA2();
}

AbstractProductB* AbstractFactory2::GetAbstractProductB()
{
    return new AbstractProductB2();
}



void AbstractFactoryTest()
{
    AbstractFactory1 *pFactory1 = new AbstractFactory1();
    AbstractProductA *pProductA = pFactory1->GetAbstractProductA();

    AbstractFactory2 *pFactory2 = new AbstractFactory2();
    AbstractProductB *pProductB = pFactory2->GetAbstractProductB();

    delete pProductB;
    delete pFactory2;

    delete pProductA;
    delete pFactory1;
}

QING_EXAMPLE_END
