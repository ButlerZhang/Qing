#include "Decorator.h"
#include <iostream>



Component::~Component()
{
    std::cout << "~Component" << std::endl;
}

ConcreteComponent::~ConcreteComponent()
{
    std::cout << "~ConcreteComponent" << std::endl;
}

void ConcreteComponent::Operation()
{
    std::cout << "ConcreteComponent::Operation" << std::endl;
}



Decorator::Decorator(Component * pCom)
{
    m_pComponent = pCom;
}

Decorator::~Decorator()
{
    std::cout << "~Decorator" << std::endl;
    delete m_pComponent;
    m_pComponent = NULL;
}

void Decorator::Operation()
{
    std::cout << "Operator::Operation" << std::endl;
}

void Decorator::SetComponent(Component * pCom)
{
    m_pComponent = pCom;
}



ConcreteDecoratorA::~ConcreteDecoratorA()
{
    std::cout << "~ConcreteDecoratorA" << std::endl;
}

void ConcreteDecoratorA::Operation()
{
    std::cout << "ConcreteDecoratorA" << std::endl;
    m_pComponent->Operation();
    AddBehavorA();
}

void ConcreteDecoratorA::AddBehavorA()
{
    std::cout << "ConcreteDecoratorA::AddBehavorA" << std::endl;
}



ConcreteDecoratorB::~ConcreteDecoratorB()
{
    std::cout << "~ConcreteDecoratorB" << std::endl;
}

void ConcreteDecoratorB::Operation()
{
    std::cout << "ConcreteDecoratorB" << std::endl;
    m_pComponent->Operation();
    AddBehavorB();
}

void ConcreteDecoratorB::AddBehavorB()
{
    std::cout << "ConcreteDecoratorB::AddBehavorB" << std::endl;
}



ConcreteDecoratorC::~ConcreteDecoratorC()
{
    std::cout << "~ConcreteDecoratorC" << std::endl;
}

void ConcreteDecoratorC::Operation()
{
    std::cout << "ConcreteDecoratorC" << std::endl;
    m_pComponent->Operation();
    AddBehavorC();
}

void ConcreteDecoratorC::AddBehavorC()
{
    std::cout << "ConcreteDecoratorC::AddBehavorC" << std::endl;
}



ConcreteDecoratorD::~ConcreteDecoratorD()
{
    std::cout << "~ConcreteDecoratorD" << std::endl;
}

void ConcreteDecoratorD::Operation()
{
    std::cout << "ConcreteDecoratorD" << std::endl;
    m_pComponent->Operation();
    AddBehavorD();
}

void ConcreteDecoratorD::AddBehavorD()
{
    std::cout << "ConcreteDecoratorD::AddBehavorD" << std::endl;
}



void TestDecorator()
{
    Component *pComponent = NULL;
    Decorator *pDecorator = NULL;

    pComponent = new ConcreteComponent();
    pDecorator = new ConcreteDecoratorA(pComponent);
    pDecorator->Operation();
    delete pDecorator;

    pComponent = new ConcreteComponent();
    pDecorator = new ConcreteDecoratorB(pComponent);
    pDecorator->Operation();
    delete pDecorator;

    pComponent = new ConcreteComponent();
    pDecorator = new ConcreteDecoratorC(pComponent);
    pDecorator->Operation();
    delete pDecorator;

    pComponent = new ConcreteComponent();
    pDecorator = new ConcreteDecoratorD(pComponent);
    pDecorator->Operation();
    delete pDecorator;
}
