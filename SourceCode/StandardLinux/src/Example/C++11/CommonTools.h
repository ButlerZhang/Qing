#pragma once
#include <tuple>
#include <ostream>
#include <iostream>
#include <complex>



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

        //tuple-like接口
        std::cout << std::get<0>(p) << std::endl;                                   //yields p.first
        std::cout << std::get<1>(p) << std::endl;                                   //yields p.second
        std::cout << std::tuple_size<IntFloatPair>::value << std::endl;             //yields 2, IntFloatPair value count
        std::tuple_element<0, IntFloatPair>::type vale = p.first;                   //yields int, get element type
    }
}

namespace C11_Tuple
{
    void Test()
    {
        std::tuple<std::string, int, int, std::complex<double>> t;                  //元素采用默认值初始化
        std::tuple<int, double, std::string> t1(41, 6.3, "nico");                   //显式初始化
        std::cout << std::get<0>(t1) << " ";
        std::cout << std::get<1>(t1) << " ";
        std::cout << std::get<2>(t1) << " " << std::endl;

        auto t2 = std::make_tuple(22, 44, "nico");                                  //类型自动推导
        std::get<1>(t1) = std::get<1>(t2);                                          //把t2.1的值赋给t1.1

        std::string s;
        std::tuple<std::string&> t3(s);                                             //元素类型可以是引用类型
        std::get<0>(t3) = "hello";

        std::tuple<int, float, std::string> t4(77, 1.1f, "more light");
        int i;
        float f;
        std::string s1;
        std::make_tuple(std::ref(i), std::ref(f), std::ref(s1)) = t4;               //这样可以提取出t的各个值
        std::tie(i, f, s) = t4;                                                     //tie比ref更方便，把t4的值赋给i、f、s1
        std::tie(i, std::ignore, std::ignore) = t4;                                 //忽略不想关注的元素

        std::tuple<int, double> t5(42, 3.14);                                       //正常
        std::tuple<int, double> t6{ 42, 3.14 };                                     //正常
        std::tuple<int, double> t7 = { 42, 3.14 };                                  //ERROR，隐式转换，VS可以编译通过?
        std::vector<std::tuple<int, double>> v{ {1,1.0},{2,2.0} };                  //ERROR，VS可以编译通过?

        typename std::tuple<int, float, std::string> TupleType;                     //不能用到下面两行，奇怪了
        std::tuple_size<std::tuple<int, float, std::string>>::value;                //元素个数
        std::tuple_element<1, std::tuple<int, float, std::string>>::type f1 = 0;    //下标为1的元素类型

        int n;
        auto t8 = std::tuple_cat(std::make_tuple(42, 7.7, "hello"), std::tie(n));   //将多个tuple串成一个
    }
}