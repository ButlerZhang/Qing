#pragma once
#include "AdapterClient.h"

//对象适配器：通过组合实现
class ObjectAdapter : public Target
{
public:

    ObjectAdapter() {
        m_pAdaptee = new Adaptee();
    }

    virtual ~ObjectAdapter() {
        delete m_pAdaptee;
        m_pAdaptee = NULL;
    }

    virtual void Request() {
        std::cout << "Object Adapter Call: ";
        m_pAdaptee->SpecificRequest();
    }

private:

    Adaptee *m_pAdaptee;
};
