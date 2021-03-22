#pragma once
#include <iostream>



//客户关注的目标接口
class Target
{
public:

    Target() {}
    virtual ~Target() {}

    virtual void Request() {
        std::cout << "Target Request." << std::endl;
    }
};



//可以实现目标接口，但现有接口不符合
class Adaptee
{
public:

    Adaptee() {}
    virtual ~Adaptee() {}

    virtual void SpecificRequest() {
        std::cout << "Adaptee SpecificRequest." << std::endl;
    }
};



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
