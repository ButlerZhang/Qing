#pragma once
#include <mutex>
#include <iostream>

std::mutex g_Lock;

class ThreadSafeSingleton {
public:

    //��Ȼ�ܱ�֤�̰߳�ȫ�����Ƕ��˼���
    //����ʵ�ֿ��Ը�Ϊʹ��RAII��ʽ������ֻ��ʾ��
    static ThreadSafeSingleton *GetInstance() {
        if (m_Instance == NULL) {
            g_Lock.lock();
            if (m_Instance == NULL) { //double check
                m_Instance = new ThreadSafeSingleton();
            }
            g_Lock.unlock();
        }

        return m_Instance;
    }

private:

    ThreadSafeSingleton(void) {
        std::cout << "Thread Safe Singleton construction." << std::endl;
    }

    ~ThreadSafeSingleton(void) {
        std::cout << "Thread Safe Singleton destruction." << std::endl;
    }

private:

    static ThreadSafeSingleton *m_Instance;
};

ThreadSafeSingleton *ThreadSafeSingleton::m_Instance = nullptr;
