#pragma once
#include "AdapterClient.h"

//����������ͨ�����ؼ̳�ʵ��
class ClassAdapter : public Target, private Adaptee
{
public:

    ClassAdapter() {}
    virtual ~ClassAdapter() {}

    virtual void Request() {
        std::cout << "Class Adapter Call: ";
        SpecificRequest();
    }
};
