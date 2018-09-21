#include "..\..\DesignPatterns\Header\Singleton.h"
#include <iostream>
#include <mutex>

QING_EXAMPLE_BEGIN



SimpleSingleton* SimpleSingleton::m_Instance = NULL;

SimpleSingleton::SimpleSingleton(void)
{
    std::cout << "SimpleSingleton construction." << std::endl;
}

SimpleSingleton::~SimpleSingleton(void)
{
    std::cout << "SimpleSingleton destruction." << std::endl;
}

SimpleSingleton* SimpleSingleton::GetInstance()
{
    if (m_Instance == NULL)
    {
        m_Instance = new SimpleSingleton();
    }

    return m_Instance;
}

void SimpleSingleton::DestroyInstance()
{
    delete m_Instance;
    m_Instance = NULL;
}



std::mutex g_Lock;
ThreadSafeSingleton* ThreadSafeSingleton::m_Instance = NULL;

ThreadSafeSingleton::ThreadSafeSingleton(void)
{
    std::cout << "ThreadSafeSingleton construction." << std::endl;
}

ThreadSafeSingleton::~ThreadSafeSingleton(void)
{
    std::cout << "ThreadSafeSingleton destruction." << std::endl;
}

ThreadSafeSingleton* ThreadSafeSingleton::GetInstance()
{
    if (m_Instance == NULL)
    {
        g_Lock.lock();
        if (m_Instance == NULL) //double check
        {
            m_Instance = new ThreadSafeSingleton();
        }
        g_Lock.unlock();
    }

    return m_Instance;
}

void ThreadSafeSingleton::DestroyInstance()
{
    delete m_Instance;
    m_Instance = NULL;
}



const UnlockSafeSingleton* UnlockSafeSingleton::m_Instance = new UnlockSafeSingleton();

UnlockSafeSingleton::UnlockSafeSingleton(void)
{
    //std::cout << "UnlockSafeSingleton construction." << std::endl;
}

UnlockSafeSingleton::~UnlockSafeSingleton(void)
{
    std::cout << "UnlockSafeSingleton destruction." << std::endl;
}

UnlockSafeSingleton* UnlockSafeSingleton::GetInstance()
{
    return const_cast<UnlockSafeSingleton*>(m_Instance);
}

void UnlockSafeSingleton::DestroyInstance()
{
    delete m_Instance;
    m_Instance = NULL;
}



Singleton::Singleton(void)
{
    std::cout << "Singleton construction." << std::endl;
}

Singleton::~Singleton(void)
{
    std::cout << "Singleton destruction." << std::endl;
}

Singleton * Singleton::GetInstance()
{
    static Singleton m_Instance;
    return &m_Instance;
}



template<typename T> TemplateSingleton<T>::TemplateSingleton(void)
{
    std::cout << "TemplateSingleton construction." << std::endl;
}

template<typename T> TemplateSingleton<T>::~TemplateSingleton(void)
{
    std::cout << "TemplateSingleton destruction." << std::endl;
}

QING_EXAMPLE_END
