#include "../HeaderFiles/FactoryMethod.h"
#include <iostream>



namespace QingTemplate
{
    MethodProductA::MethodProductA()
    {
        std::cout << "construction of MethodProductA." << std::endl;
    }

    MethodProductA::~MethodProductA()
    {
        std::cout << "destruction of MethodProductA." << std::endl;
    }

    MethodProductB::MethodProductB()
    {
        std::cout << "construction of MethodProductB." << std::endl;
    }

    MethodProductB::~MethodProductB()
    {
        std::cout << "destruction of MethodProductB." << std::endl;
    }



    FactoryMethodA::FactoryMethodA()
    {
        std::cout << "construction of FactoryMethodA" << std::endl;
    }

    FactoryMethodA::~FactoryMethodA()
    {
        std::cout << "destruction of FactoryMethodA" << std::endl;
    }

    MethodProduct* FactoryMethodA::GetMethodProduct()
    {
        return new MethodProductA();
    }

    FactoryMethodB::FactoryMethodB()
    {
        std::cout << "construction of FactoryMethodB." << std::endl;
    }

    FactoryMethodB::~FactoryMethodB()
    {
        std::cout << "destruction of FactoryMethodB." << std::endl;
    }

    MethodProduct* FactoryMethodB::GetMethodProduct()
    {
        return new MethodProductB();
    }



    void FactoryMethodTest()
    {
        FactoryMethod *f = new FactoryMethodA();
        MethodProduct* p = f->GetMethodProduct();

        delete p;
        delete f;
    }
}
