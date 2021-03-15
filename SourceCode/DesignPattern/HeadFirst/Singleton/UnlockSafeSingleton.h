#pragma once
#include<iostream>

class UnlockSafeSingleton
{
public:

    //需要类型转换，性能欠缺
    static UnlockSafeSingleton* GetInstance() {
        return const_cast<UnlockSafeSingleton*>(m_Instance);
    }

private:

    UnlockSafeSingleton(void) {
        std::cout << "Unlock Safe Singleton construction." << std::endl;
    }

    ~UnlockSafeSingleton(void) {
        std::cout << "Unlock Safe Singleton destruction." << std::endl;
    }

private:

    static const UnlockSafeSingleton* m_Instance;
};

const UnlockSafeSingleton* UnlockSafeSingleton::m_Instance = new UnlockSafeSingleton();
