#include "Adapter.h"
#include <iostream>

QING_EXAMPLE_BEGIN



void Target::Request()
{
    std::cout << "Target Request." << std::endl;
}

void Adaptee::SpecificRequest()
{
    std::cout << "Adaptee SpecificRequest." << std::endl;
}

void ClassAdapter::Request()
{
    std::cout << "Class Adapter Call: ";
    SpecificRequest();
}



ObjectAdapter::ObjectAdapter()
{
    m_pAdaptee = new Adaptee();
}

ObjectAdapter::~ObjectAdapter()
{
    delete m_pAdaptee;
    m_pAdaptee = NULL;
}

void ObjectAdapter::Request()
{
    std::cout << "Object Adapter Call: ";
    m_pAdaptee->SpecificRequest();
}



void TestAdapter()
{
    ClassAdapter MyClassAdapter;
    MyClassAdapter.Request();

    ObjectAdapter MyObjectAdapter;
    MyObjectAdapter.Request();
}

QING_EXAMPLE_END
