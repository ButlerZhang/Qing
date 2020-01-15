#pragma once
#include <random>



class RandomGenerator
{
public:

    RandomGenerator(void);
    RandomGenerator(unsigned int Seed);
    ~RandomGenerator();

    long unsigned int GetRandomUInt();
    long unsigned int GetRandomUIntInRange(int Min, int Max);

    double GetRandomDouble();
    double GetRandomDoubleInRange(double Min, double Max);

private:

    std::default_random_engine m_Engine;
};
