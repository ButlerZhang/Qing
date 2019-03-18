#include "RandomGenerator.h"
#include <ctime>

QING_NAMESPACE_BEGIN



RandomGenerator::RandomGenerator(void)
{
    m_Engine.seed(static_cast<unsigned int>(time(NULL)));
}

RandomGenerator::RandomGenerator(unsigned int Seed)
{
    //use same seed will generate same numbers sequence
    m_Engine.seed(Seed);
}

RandomGenerator::~RandomGenerator()
{
}

unsigned int RandomGenerator::GetRandomUInt()
{
    return m_Engine();
}

unsigned int RandomGenerator::GetRandomUIntInRange(int Min, int Max)
{
    static std::uniform_int_distribution<unsigned int> u(Min, Max);
    return u(m_Engine);
}

double RandomGenerator::GetRandomDouble()
{
    return GetRandomDoubleInRange(0.0, 1.0);
}

double RandomGenerator::GetRandomDoubleInRange(double Min, double Max)
{
    static std::uniform_real_distribution<double> u(Min, Max);
    return u(m_Engine);
}

QING_NAMESPACE_END