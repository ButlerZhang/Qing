#pragma once
#include <iostream>

//�ͻ���ע��Ŀ��ӿ�
class Target {
public:

    Target() {}
    virtual ~Target() {}

    virtual void Request() {
        std::cout << "Target Request." << std::endl;
    }
};



//����ʵ��Ŀ��ӿڣ������нӿڲ�����
class Adaptee {
public:

    Adaptee() {}
    virtual ~Adaptee() {}

    virtual void SpecificRequest() {
        std::cout << "Adaptee SpecificRequest." << std::endl;
    }
};
