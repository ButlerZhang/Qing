#pragma once
#include <list>
#include <vector>
#include <string>
#include <complex>
#include <iostream>
#include <array>
#include <bitset>
#include <map>



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

    //因为需要根据初值推导类型，所以必须初始化
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
    //用以避免非必要的拷贝和临时对象。
    //std::move不是真正的搬迁内容，本质上是Rvalue Reference。
    //被搬迁之后，原对象的内容应该为空，但有的编译器仍可能使用原对象。
}

//7.新式的字符串字面常量(String Literal)
namespace C11_7
{
    //Raw String Literal,类似于python中r开头的字符串,这样可以避免转义。
    //完整语法：R"delim(...)delim"
    std::string test = R"(\\\\\n)";
}

//8.关键字noexcept
namespace C11_8
{
    //指明某个函数不抛出异常。
    //如果foo最终抛出了异常，程序会被终止。
    void foo() noexcept {}

    //noexcept(bool)
    //如果x.swap(y)不抛出异常，swap_test就不抛出异常
    template<class Type>
    void swap_test(Type &x, Type &y) noexcept(noexcept(x.swap(y)))
    {
        x.swap(y);
    }
}

//9.关键字constexpr
namespace C11_9
{
    //constexpr将表达式核定于编译期
    constexpr int square(int x) { return x*x; }
    float a[square(9)];                     //OK, a has 81 elements
}

//10.崭新的Template特性
namespace C11_10
{
    //Variadic Template
    void Print() {}

    template<typename T, typename ... Types>
    void Print(const T& firstArg, const Types&& ...args)
    {
        std::cout << firstArg << std::endl; //print first argument
        print(args...);                     //call print() for remaining arguments
    }

    void Test() { Print(7, "hello", std::bitset<16>(233), 3.0); }

    //Alias Tempalte(带别名的模板,或称为Template Typedef)

    //定义
    template< typename T>
    using Vec = std::vector<T, MyAlloc<T>>;

    //声明
    Vec<int> Coll;

    //上式等价于
    std::vector<int, MyAlloc<int>> Coll;
}

//11.Lambda
namespace C11_11
{
    //lambda允许inline函数的定义式被用作一个参数，或是一个Local对象。

    //最小型的lambda:
    [] {std::cout << "Hello lambda" << std::endl; }();

    //添加传入的参数
    auto l1 = [](const std::string &s) {std::cout << s << std::endl; };
    l1("hello lambda");

    //可以不写返回类型，会自动推导
    [] { return 32; };

    //可以明确指出返回类型
    []()->doubles { return 32; };

    //Capture用以访问外部作用域
    int x = 0;
    int y = 42;
    auto l2 = [x, &y] {
        std::cout << x << std::endl;
        std::cout << y++ << std::endl;
    };

    l2(x, y);

    //lambda的类型
    std::is_function<int(int, int)> ReturnLambda()
    {
        return [](int x, int y) {return x*y; };
    }
}

//12.关键字decltype
namespace C11_12
{
    //获取表达式的类型。可以替换缺乏一致性又不完全的typeof。
    std::map<std::string, float> coll;
    decltype(coll)::value_type elem;
}

//13.新的函数声明语法(New Function Declaration Syntax)
namespace C11_13
{
    //可以将函数的返回类型声明于参数列之后
    template<typename T1, typename T2>
    auto add(T1 x, T2 y) -> decltype(x + y) {}
}

//14.带领域的(scoped)Enumeration
namespace C11_14
{
    enum class Salutation : char { mr, ms, co, none };
}

//15.新的基础类型
namespace C11_15
{
    char16_t c16;
    char32_t c32;
    long long ll;
    unsigned long long ull;
    std::nullptr_t t;
}

