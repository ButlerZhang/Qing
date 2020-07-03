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

        //tuple-like�ӿ�
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
        std::tuple<std::string, int, int, std::complex<double>> t;                  //Ԫ�ز���Ĭ��ֵ��ʼ��
        std::tuple<int, double, std::string> t1(41, 6.3, "nico");                   //��ʽ��ʼ��
        std::cout << std::get<0>(t1) << " ";
        std::cout << std::get<1>(t1) << " ";
        std::cout << std::get<2>(t1) << " " << std::endl;

        auto t2 = std::make_tuple(22, 44, "nico");                                  //�����Զ��Ƶ�
        std::get<1>(t1) = std::get<1>(t2);                                          //��t2.1��ֵ����t1.1

        std::string s;
        std::tuple<std::string&> t3(s);                                             //Ԫ�����Ϳ�������������
        std::get<0>(t3) = "hello";

        std::tuple<int, float, std::string> t4(77, 1.1f, "more light");
        int i;
        float f;
        std::string s1;
        std::make_tuple(std::ref(i), std::ref(f), std::ref(s1)) = t4;               //����������ȡ��t�ĸ���ֵ
        std::tie(i, f, s) = t4;                                                     //tie��ref�����㣬��t4��ֵ����i��f��s1
        std::tie(i, std::ignore, std::ignore) = t4;                                 //���Բ����ע��Ԫ��

        std::tuple<int, double> t5(42, 3.14);                                       //����
        std::tuple<int, double> t6{ 42, 3.14 };                                     //����
        std::tuple<int, double> t7 = { 42, 3.14 };                                  //ERROR����ʽת����VS���Ա���ͨ��?
        std::vector<std::tuple<int, double>> v{ {1,1.0},{2,2.0} };                  //ERROR��VS���Ա���ͨ��?

        typename std::tuple<int, float, std::string> TupleType;                     //�����õ��������У������
        std::tuple_size<std::tuple<int, float, std::string>>::value;                //Ԫ�ظ���
        std::tuple_element<1, std::tuple<int, float, std::string>>::type f1 = 0;    //�±�Ϊ1��Ԫ������

        int n;
        auto t8 = std::tuple_cat(std::make_tuple(42, 7.7, "hello"), std::tie(n));   //�����tuple����һ��
    }
}