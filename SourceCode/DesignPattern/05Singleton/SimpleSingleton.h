#pragma once
#include <iostream>

class SimpleSingleton {
public:

    //���߳����޷���֤Ψһ��
    //����ʵ�ַ�ʽ��Ҫ����ʹ��
    static SimpleSingleton *GetInstance() {
        if (m_Instance == NULL) {
            m_Instance = new SimpleSingleton();
        }

        return m_Instance;
    }

private:

    SimpleSingleton(void) {
        std::cout << "Simple Singleton construction." << std::endl;
    }

    ~SimpleSingleton(void) {
        std::cout << "Simple Singleton destruction." << std::endl;
    }

private:

    static SimpleSingleton *m_Instance;
};

SimpleSingleton *SimpleSingleton::m_Instance = nullptr;
