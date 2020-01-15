#include <iostream>
#include "src/BoostTime.h"
#include "src/BoostFileSystem.h"



int main(int argc, char *argv[])
{
    BoostTime Time;
    std::cout << BoostFileSystem::GetWorkDirectory() << std::endl;
    std::cout << Time.ElapsedMicroseconds() << std::endl;

    int x = 0;
    std::cin >> x;
    std::cout << x << std::endl;

    return 0;
}
