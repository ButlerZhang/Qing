#include <iostream>
#include "src/RandomGenerator.h"



int main(int argc, char *argv[])
{
    RandomGenerator RG;
    for (int i = 0; i < 10; i++)
    {
        std::cout << RG.GetRandomUInt() << std::endl;
    }

    int x = 0;
    std::cin >> x;

    return 0;
}
