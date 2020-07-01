#pragma once
#include <list>
#include <vector>
#include <string>
#include <complex>
#include <iostream>
#include <array>
#include <bitset>
#include <map>



//1.Template���ʽ�ڵĿո����ȡ��
namespace C11_1
{
    std::vector<std::list<int> >;           //OK in each C++ version
    std::vector<std::list<int>>;            //OK since C++11
}

//2.nullptr��������std::nullptr_t
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

//3.auto��������Ƶ�
namespace C11_3
{
    auto i = 42;                            //i has type int

    double f() { return 0.0; }
    auto d = f();                           //d has type double

    //��Ϊ��Ҫ���ݳ�ֵ�Ƶ����ͣ����Ա����ʼ��
    auto j;                                 //ERROR, must init

    static auto vat = 0.19;                 //can add static

    std::vector<std::string> vec;
    auto pos = vec.begin();                 //pos has type std::vector<std::string>::iterator

    //��������д�������ľ������ͣ�auto���ֻ����lambda�ϡ�
    auto l = [](int x)-> bool {};           //l has type lambda
}

//4.һ���Գ�ʼ��(Uniform Initialization)���ֵ��(Initializer List)
namespace C11_4
{
    //ʹ�ô�����
    int values[]{ 1,2,3 };
    std::vector<int> v{ 1,2,3,4,5,6,7 };
    std::vector<std::string> cities{ "Berlin", "New York","London" };
    std::complex<double> c{ 4.0,3.0 };

    //initializer list ��ǿ��ʹ�� value initialization
    int i;                                  //i has undefined value
    int j{};                                //j is initialized by 0
    int *p;                                 //p has undefined value
    int *q{};                               //q is initialized by nullptr

    //�����Ų�����խ��narrowing
    int x1(5.3);                            //OK, but x1 becomes 5
    int x2 = 5.3;                           //OK, but x2 becomes 5
    int x3{ 5.3 };                          //ERROR, narrowing
    int x4 = { 5.3 };                       //ERROR, narrowing
    char c1{ 7 };                           //OK
    char c2{ 9999 };                        //ERROR, 9999 doesn't fix into a char
    std::vector<int> v1{ 1,2,3,4,5 };       //OK
    std::vector<int> v2{ 1,2.3,4,5.6 };     //ERROR

    //ʹ��std::initializer_list֧��һϵ��ֵ�ĳ�ʼ��
    void print_f(std::initializer_list<int> vals)
    {
        for (auto p = vals.begin(); p != vals.end(); ++p)
        {
            std::cout << *p << "\n";
        }
    }

    //����ʹ��std::initializer_list
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

//5.Range-Based forѭ��, ��foreachѭ��
namespace C11_5
{
    //��ʽΪ��
    //      for (decl : coll) {
    //          statement
    //      }

    //�����ϵ�ͬ�ڣ�
    //      for(auto pos = coll.begin(); pos != coll.end(); ++pos){
    //          decl = *pos;
    //          statement;
    //      }

    void test()
    {
        //��ӡԪ��
        for (int i : {1, 2, 3, 4, 5, 6, 7})
        {
            std::cout << i << std::endl;
        }

        //�޸�Ԫ��
        std::vector<double> vec;
        for (auto& elem : vec)
        {
            elem *= 3;
        }

        //��ӡģ��
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

//6.Move�����Rvalue Reference
namespace C11_6
{
    //���Ա���Ǳ�Ҫ�Ŀ�������ʱ����
    //std::move���������İ�Ǩ���ݣ���������Rvalue Reference��
    //����Ǩ֮��ԭ���������Ӧ��Ϊ�գ����еı������Կ���ʹ��ԭ����
}

//7.��ʽ���ַ������泣��(String Literal)
namespace C11_7
{
    //Raw String Literal,������python��r��ͷ���ַ���,�������Ա���ת�塣
    //�����﷨��R"delim(...)delim"
    std::string test = R"(\\\\\n)";
}

//8.�ؼ���noexcept
namespace C11_8
{
    //ָ��ĳ���������׳��쳣��
    //���foo�����׳����쳣������ᱻ��ֹ��
    void foo() noexcept {}

    //noexcept(bool)
    //���x.swap(y)���׳��쳣��swap_test�Ͳ��׳��쳣
    template<class Type>
    void swap_test(Type &x, Type &y) noexcept(noexcept(x.swap(y)))
    {
        x.swap(y);
    }
}

//9.�ؼ���constexpr
namespace C11_9
{
    //constexpr�����ʽ�˶��ڱ�����
    constexpr int square(int x) { return x*x; }
    float a[square(9)];                     //OK, a has 81 elements
}

//10.ո�µ�Template����
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

    //Alias Tempalte(��������ģ��,���ΪTemplate Typedef)

    //����
    template< typename T>
    using Vec = std::vector<T, MyAlloc<T>>;

    //����
    Vec<int> Coll;

    //��ʽ�ȼ���
    std::vector<int, MyAlloc<int>> Coll;
}

//11.Lambda
namespace C11_11
{
    //lambda����inline�����Ķ���ʽ������һ������������һ��Local����

    //��С�͵�lambda:
    [] {std::cout << "Hello lambda" << std::endl; }();

    //��Ӵ���Ĳ���
    auto l1 = [](const std::string &s) {std::cout << s << std::endl; };
    l1("hello lambda");

    //���Բ�д�������ͣ����Զ��Ƶ�
    [] { return 32; };

    //������ȷָ����������
    []()->doubles { return 32; };

    //Capture���Է����ⲿ������
    int x = 0;
    int y = 42;
    auto l2 = [x, &y] {
        std::cout << x << std::endl;
        std::cout << y++ << std::endl;
    };

    l2(x, y);

    //lambda������
    std::is_function<int(int, int)> ReturnLambda()
    {
        return [](int x, int y) {return x*y; };
    }
}

//12.�ؼ���decltype
namespace C11_12
{
    //��ȡ���ʽ�����͡������滻ȱ��һ�����ֲ���ȫ��typeof��
    std::map<std::string, float> coll;
    decltype(coll)::value_type elem;
}

//13.�µĺ��������﷨(New Function Declaration Syntax)
namespace C11_13
{
    //���Խ������ķ������������ڲ�����֮��
    template<typename T1, typename T2>
    auto add(T1 x, T2 y) -> decltype(x + y) {}
}

//14.�������(scoped)Enumeration
namespace C11_14
{
    enum class Salutation : char { mr, ms, co, none };
}

//15.�µĻ�������
namespace C11_15
{
    char16_t c16;
    char32_t c32;
    long long ll;
    unsigned long long ull;
    std::nullptr_t t;
}

