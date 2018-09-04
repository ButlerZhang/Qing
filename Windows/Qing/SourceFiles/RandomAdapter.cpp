#include "..\HeaderFiles\RandomAdapter.h"
#include <ctime>

QING_NAMESPACE_BEGIN



RandomAdapter::RandomAdapter(void)
{
    m_engine.seed(static_cast<unsigned int>(time(NULL)));
}

RandomAdapter::RandomAdapter(unsigned int seed)
{
    //use same seed will generate same numbers sequence
    m_engine.seed(seed);
}

RandomAdapter::~RandomAdapter()
{
}

unsigned long RandomAdapter::GetRandom()
{
    return m_engine();
}

int RandomAdapter::GetGetRandom(int min, int max)
{
    if (min < max)
    {
        std::uniform_int_distribution<unsigned> u(0, 9);
        return u(m_engine);
    }

    return 0;
}

QING_NAMESPACE_END