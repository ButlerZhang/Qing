#include "..\HeaderFiles\RandomGenerator.h"
#include <ctime>

QING_NAMESPACE_BEGIN



RandomGenerator::RandomGenerator(void)
{
    m_engine.seed(static_cast<unsigned int>(time(NULL)));
}

RandomGenerator::RandomGenerator(unsigned int seed)
{
    //use same seed will generate same numbers sequence
    m_engine.seed(seed);
}

RandomGenerator::~RandomGenerator()
{
}

unsigned int RandomGenerator::GetRandomUInt()
{
    return m_engine();
}

unsigned int RandomGenerator::GetRandomUIntInRange(int min, int max)
{
    static std::uniform_int_distribution<unsigned int> u(min, max);
    return u(m_engine);
}

double RandomGenerator::GetRandomDouble()
{
    return GetRandomDoubleInRange(0.0, 1.0);
}

double RandomGenerator::GetRandomDoubleInRange(double min, double max)
{
    static std::uniform_real_distribution<double> u(min, max);
    return u(m_engine);
}

QING_NAMESPACE_END