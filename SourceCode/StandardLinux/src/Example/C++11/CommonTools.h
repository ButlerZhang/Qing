#pragma once
#include <tuple>
#include <ostream>
#include <iostream>



namespace C11_Pair
{
    template<typename T1, typename T2>
    std::ostream& operator << (std::ostream &strm, const std::pair<T1, T2> &p)
    {
        return strm << "[" << p.first << "," << p.second << "]";
    }

    void Test()
    {
        typedef std::pair<int, float> IntFloatPair;
        IntFloatPair p(42, 3.14f);

        std::cout << std::get<0>(p) << std::endl;                           //yields p.first
        std::cout << std::get<1>(p) << std::endl;                           //yields p.second
        std::cout << std::tuple_size<IntFloatPair>::value << std::endl;     //yields 2, IntFloatPair value count
        std::tuple_element<0, IntFloatPair>::type vale = p.first;           //yields int, get element type
    }
}
