#pragma once
#include <iostream>

class NormalSingleton {
public:

    //常见的实现方式，大部分情况下够用了
    static NormalSingleton *GetInstance() {
        static NormalSingleton m_Instance;
        return &m_Instance;
    }

private:

    NormalSingleton(void) {
        std::cout << "Normal Singleton construction." << std::endl;
    }

    ~NormalSingleton(void) {
        std::cout << "Normal Singleton destruction." << std::endl;
    }
};
