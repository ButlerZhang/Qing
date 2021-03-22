#pragma once
#include <iostream>



//�ͻ���ע��Ŀ��ӿ�
class Target
{
public:

    Target() {}
    virtual ~Target() {}

    virtual void Request() {
        std::cout << "Target Request." << std::endl;
    }
};



//����ʵ��Ŀ��ӿڣ������нӿڲ�����
class Adaptee
{
public:

    Adaptee() {}
    virtual ~Adaptee() {}

    virtual void SpecificRequest() {
        std::cout << "Adaptee SpecificRequest." << std::endl;
    }
};



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
