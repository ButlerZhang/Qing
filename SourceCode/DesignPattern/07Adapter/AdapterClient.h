#pragma once
#include <iostream>

//客户关注的目标接口
class Target {
public:

    Target() {}
    virtual ~Target() {}

    virtual void Request() {
        std::cout << "Target Request." << std::endl;
    }
};



//可以实现目标接口，但现有接口不符合
class Adaptee {
public:

    Adaptee() {}
    virtual ~Adaptee() {}

    virtual void SpecificRequest() {
        std::cout << "Adaptee SpecificRequest." << std::endl;
    }
};
