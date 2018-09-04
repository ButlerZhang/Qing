#pragma once
#include "QingBase.h"
#include <random>
#include <memory>

QING_NAMESPACE_BEGIN



class QING_DLL RandomAdapter
{
public:

    RandomAdapter(void);
    RandomAdapter(unsigned int seed);
    ~RandomAdapter();

    unsigned long GetRandom();
    int GetGetRandom(int min, int max);

private:

    std::default_random_engine m_engine;
};

QING_NAMESPACE_END