#pragma once
#include "AdapterClient.h"

//类适配器：通过多重继承实现
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
