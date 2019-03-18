#include "SimpleFactory.h"
#include <iostream>



SimpleProductA::SimpleProductA()
{
    std::cout << "construction of SimpleProductA." << std::endl;
}

SimpleProductA::~SimpleProductA()
{
    std::cout << "destruction of SimpleProductA." << std::endl;
}

SimpleProductB::SimpleProductB()
{
    std::cout << "construction of SimpleProductB." << std::endl;
}

SimpleProductB::~SimpleProductB()
{
    std::cout << "destruction of SimpleProductB." << std::endl;
}



SimpleProduct* SimpleFactory::GetSimpleProduct(SimpleProductType ProductType)
{
    switch (ProductType)
    {
    case SPT_A:     return new SimpleProductA();
    case SPT_B:     return new SimpleProductB();
        //case SPT_C:     return new SimpleProductC();
        //case SPT_D:     return new SImpleProductD();
    default:        return NULL;
    }
}



void SimpleFactoryTest()
{
    SimpleFactory MySimpleFactory;
    SimpleProduct *p = MySimpleFactory.GetSimpleProduct(SPT_A);
    delete p;
}

