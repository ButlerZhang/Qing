#pragma once
#include <list>
#include <vector>
#include <string>
#include <complex>
#include <iostream>



//1.Template表达式内的空格可以取消
namespace C11_1
{
    std::vector<std::list<int> >;           //OK in each C++ version
    std::vector<std::list<int>>;            //OK since C++11
}

//2.nullptr的类型是std::nullptr_t
namespace C11_2
{
#define NULL 0

    void f(int) {}
    void f(void*) {}

    void test()
    {
        f(0);                               //calls f(int)
        f(NULL);                            //calls f(int) is NULL is 0, ambiguous otherwise
        f(nullptr);                         //calls f(void*)
    }
}

//3.auto完成类型推导
namespace C11_3
{
    auto i = 42;                            //i has type int

    double f() { return 0.0; }
    auto d = f();                           //d has type double

    auto j;                                 //ERROR, must init

    static auto vat = 0.19;                 //can add static

    std::vector<std::string> vec;
    auto pos = vec.begin();                 //pos has type std::vector<std::string>::iterator

    //我倾向于写出变量的具体类型，auto最好只用在lambda上。
    auto l = [](int x)-> bool {};           //l has type lambda
}

//4.一致性初始化(Uniform Initialization)与初值列(Initializer List)
namespace C11_4
{
    //使用大括号
    int values[]{ 1,2,3 };
    std::vector<int> v{ 1,2,3,4,5,6,7 };
    std::vector<std::string> cities{ "Berlin", "New York","London" };
    std::complex<double> c{ 4.0,3.0 };

    //initializer list 会强制使用 value initialization
    int i;                                  //i has undefined value
    int j{};                                //j is initialized by 0
    int *p;                                 //p has undefined value
    int *q{};                               //q is initialized by nullptr

    //大括号不允许窄化narrowing
    int x1(5.3);                            //OK, but x1 becomes 5
    int x2 = 5.3;                           //OK, but x2 becomes 5
    int x3{ 5.3 };                          //ERROR, narrowing
    int x4 = { 5.3 };                       //ERROR, narrowing
    char c1{ 7 };                           //OK
    char c2{ 9999 };                        //ERROR, 9999 doesn't fix into a char
    std::vector<int> v1{ 1,2,3,4,5 };       //OK
    std::vector<int> v2{ 1,2.3,4,5.6 };     //ERROR

    //使用std::initializer_list支持一系列值的初始化
    void print_f(std::initializer_list<int> vals)
    {
        for (auto p = vals.begin(); p != vals.end(); ++p)
        {
            std::cout << *p << "\n";
        }
    }

    //优先使用std::initializer_list
    class Print1
    {
    public:
        Print1(int, int) {}
        Print1(std::initializer_list<int>) {}
    };

    //explicit
    class Print2
    {
    public:
        Print2(int, int) {}
        explicit Print2(int, int, int) {}
    };

    void test()
    {
        print_f({ 1,2,3,4,5 });             //calls function

        Print1 p1(1, 2);                    //calls (int,int)
        Print1 p2{ 1, 2 };                  //calls std::initializer_list
        Print1 p3{ 1,2,3,4 };               //calls std::initializer_list
        Print1 p4 = { 1,2 };                //calls std::initializer_list

        Print2 p1(1, 2);                    //OK
        Print2 p2{ 1, 2 };                  //OK
        Print2 p3{ 1,2,3 };                 //OK
        Print2 p4 = { 1,2 };                //OK
        Print2 p5 = { 1,2,3 };              //ERROR due to explicit

        void fp(const Print2&) {}
        fp({ 1,2 });                        //OK, implicit conversion of {1,2} into Print2
        fp({ 1,2,3 });                      //ERROR due to explicit
        fp(Print2{ 1,2 });                  //OK, explicit conversion of {1,2} into Print2
        fp(Print2{ 1,2,3 });                //OK, explicit conversion of {1,2,3} into Print2
    }
}

//5.Range-Based for循环, 即foreach循环
namespace C11_5
{
    //格式为：
    //      for (decl : coll) {
    //          statement
    //      }

    //本质上等同于：
    //      for(auto pos = coll.begin(); pos != coll.end(); ++pos){
    //          decl = *pos;
    //          statement;
    //      }

    void test()
    {
        //打印元素
        for (int i : {1, 2, 3, 4, 5, 6, 7})
        {
            std::cout << i << std::endl;
        }

        //修改元素
        std::vector<double> vec;
        for (auto& elem : vec)
        {
            elem *= 3;
        }

        //打印模板
        template <typename T>
        void PrintElements(const T& coll)
        {
            for (const auto& elem : coll)
            {
                std::cout << elem << std::endl;
            }
        }
    }
}

//6.Move语义和Rvalue Reference
namespace C11_6
{

}










