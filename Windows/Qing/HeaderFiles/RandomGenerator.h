#pragma once
#include "QingBase.h"
#include <random>

QING_NAMESPACE_BEGIN



class QING_DLL RandomGenerator
{
public:

    RandomGenerator(void);
    RandomGenerator(unsigned int seed);
    ~RandomGenerator();

    unsigned int GetRandomUInt();
    unsigned int GetRandomUIntInRange(int min, int max);

    double GetRandomDouble();
    double GetRandomDoubleInRange(double min, double max);

private:

    std::default_random_engine m_engine;
};

QING_NAMESPACE_END