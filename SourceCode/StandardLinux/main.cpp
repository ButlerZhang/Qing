#include <iostream>
#include "src/RandomGenerator.h"
#include "src/Example/C++11/CommonTools.h"



int main(int argc, char *argv[])
{
    {
        //RandomGenerator RG;
        //for (int i = 0; i < 10; i++)
        //{
        //    std::cout << RG.GetRandomUInt() << std::endl;
        //}

        //C11_SmartPointer::SharedPointer();
        //C11_SmartPointer::WeakPointer();
        //C11_NumericLimit::Test();
        //C11_Ratio::Test();
        //C11_Chrono::TestDuration();
        //C11_Chrono::TestClock();
        C11_Chrono::TestTimepoint();
    }

    int x = 0;
    std::cin >> x;

    return 0;
}
