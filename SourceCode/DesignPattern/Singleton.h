#pragma once



class SimpleSingleton
{
public:

    static SimpleSingleton* GetInstance();
    static void DestroyInstance();

private:

    SimpleSingleton(void);
    ~SimpleSingleton(void);

private:

    static SimpleSingleton *m_Instance;
};



class ThreadSafeSingleton
{
public:

    static ThreadSafeSingleton* GetInstance();
    static void DestroyInstance();

private:

    ThreadSafeSingleton(void);
    ~ThreadSafeSingleton(void);

private:

    static ThreadSafeSingleton *m_Instance;
};



class UnlockSafeSingleton
{
public:

    static UnlockSafeSingleton* GetInstance();
    static void DestroyInstance();

private:

    UnlockSafeSingleton(void);
    ~UnlockSafeSingleton(void);

private:

    static const UnlockSafeSingleton *m_Instance;
};



class Singleton
{
public:

    static Singleton* GetInstance();

private:

    Singleton(void);
    ~Singleton(void);
};


template<typename T>
class TemplateSingleton
{
public:

    static T* GetInstance()
    {
        static T m_Instance;
        return &m_Instance;
    }

private:

    TemplateSingleton(void);
    ~TemplateSingleton(void);
};
