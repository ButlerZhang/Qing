#pragma once
#include <iostream>

class SimpleSingleton {
public:

    //多线程下无法保证唯一性
    //这种实现方式需要谨慎使用
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
