#pragma once
#include <iostream>

class NormalSingleton {
public:

    //������ʵ�ַ�ʽ���󲿷�����¹�����
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
