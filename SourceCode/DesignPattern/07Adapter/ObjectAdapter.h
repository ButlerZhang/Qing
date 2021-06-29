#pragma once
#include "AdapterClient.h"

//������������ͨ�����ʵ��
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
