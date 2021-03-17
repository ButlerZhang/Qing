#pragma once
#include <iostream>

template<typename T>
class TemplateSingleton
{
public:

    static T* GetInstance() {
        static T m_Instance;
        return &m_Instance;
    }

private:

    TemplateSingleton(void) {
        std::cout << "Template Singleton construction." << std::endl;
    }

    ~TemplateSingleton(void) {
        std::cout << "Template Singleton destruction." << std::endl;
    }
};
