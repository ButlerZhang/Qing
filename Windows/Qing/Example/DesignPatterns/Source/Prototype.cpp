#include "..\..\DesignPatterns\Header\Prototype.h"
#include <iostream>

QING_EXAMPLE_BEGIN



PrototypeA::PrototypeA(void)
{
    std::cout << "construction of PrototypeA." << std::endl;
}

PrototypeA::PrototypeA(const PrototypeA&)
{
    std::cout << "copy construction of PrototypeA." << std::endl;
}

PrototypeA::~PrototypeA(void)
{
    std::cout << "destruction of PrototypeA." << std::endl;
}

Prototype* PrototypeA::Clone() const
{
    return new PrototypeA(*this);
}



PrototypeB::PrototypeB(void)
{
    std::cout << "construction of PrototypeB." << std::endl;
}

PrototypeB::PrototypeB(const PrototypeB&)
{
    std::cout << "copy construction of PrototypeB." << std::endl;
}

PrototypeB::~PrototypeB(void)
{
    std::cout << "destruction of PrototypeB." << std::endl;
}

Prototype* PrototypeB::Clone() const
{
    return new PrototypeB(*this);
}



void ProtoTypeTest()
{
    Prototype *pProtoTypeA = new PrototypeA();
    Prototype *pProtoTypeA1 = pProtoTypeA->Clone();

    Prototype *pProtoTypeB = new PrototypeB();
    Prototype *pProtoTypeB1 = pProtoTypeB->Clone();

    delete pProtoTypeB1;
    delete pProtoTypeB;

    delete pProtoTypeA1;
    delete pProtoTypeA;
}

QING_EXAMPLE_END
