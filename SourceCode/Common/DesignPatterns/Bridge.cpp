#include "Bridge.h"
#include <iostream>



void ConcreteImplementorA::OperatorImpl()
{
    std::cout << "Implementation by ConcreteImplementForA." << std::endl;
}

void ConcreteImplementorB::OperatorImpl()
{
    std::cout << "Implementation by ConcreteImplementForB." << std::endl;
}



RefineAbstractionA::RefineAbstractionA(Implementor *pImplementor) : Abstraction(pImplementor)
{
    m_Implementor = pImplementor;
}

RefineAbstractionA::~RefineAbstractionA()
{
    delete m_Implementor;
    m_Implementor = NULL;
}

void RefineAbstractionA::Operator()
{
    m_Implementor->OperatorImpl();
}



RefineAbstractionB::RefineAbstractionB(Implementor *pImplementor) : Abstraction(pImplementor)
{
    m_Implementor = pImplementor;
}

RefineAbstractionB::~RefineAbstractionB()
{
    delete m_Implementor;
    m_Implementor = NULL;
}

void RefineAbstractionB::Operator()
{
    m_Implementor->OperatorImpl();
}



void TestBridge()
{
    ConcreteImplementorA *pImplA = new ConcreteImplementorA();
    Abstraction *pAbstraction1 = new RefineAbstractionA(pImplA);
    pAbstraction1->Operator();

    ConcreteImplementorB *pImplB = new ConcreteImplementorB();
    Abstraction *pAbstraction2 = new RefineAbstractionB(pImplB);
    pAbstraction2->Operator();
}
