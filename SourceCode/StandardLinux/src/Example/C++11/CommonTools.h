#pragma once
#include <tuple>
#include <ostream>
#include <iostream>
#include <complex>
#include <memory>
#include <vector>
#include <string>
#include <limits>
#include <ratio>
#include <chrono>
#include <ctime>



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

namespace C11_SmartPointer
{
    void SharedPointer()
    {
        //two shared pointers representing two persons by their name
        std::shared_ptr<std::string> pNico(new std::string("nico"));
        std::shared_ptr<std::string> pJutta(new std::string("jutta"));

        //其它初始化方法
        //std::shared_ptr<std::string> pNico1 = new std::string("nico");                //ERROR, explicit
        //std::shared_ptr<std::string> pNico2{ new std::string("nico") }                //OK
        //std::shared_ptr<std::string> pNico3 = std::make_shared<std::string>("Nico");  //OK

        //std::shared_ptr<std::string> pNico4;
        //pNico4 = new std::string("Nico");                                             //ERROR, explicit
        //pNico4.reset(new std::string("Nico"));                                        //OK

        //capitalize person names
        (*pNico)[0] = 'N';
        pJutta->replace(0, 1, "J");

        //put them multiple times in a container
        std::vector<std::shared_ptr<std::string>> whoMadeCoffee;
        whoMadeCoffee.push_back(pJutta);
        whoMadeCoffee.push_back(pJutta);
        whoMadeCoffee.push_back(pNico);
        whoMadeCoffee.push_back(pJutta);
        whoMadeCoffee.push_back(pNico);

        //print all elements
        for (auto ptr : whoMadeCoffee)
        {
            std::cout << *ptr << " ";
        }
        std::cout << std::endl;

        //overwrite a name agian
        *pNico = "Nicolai";

        //print all elements again
        for (auto ptr : whoMadeCoffee)
        {
            std::cout << *ptr << " ";
        }
        std::cout << std::endl;

        //print some internal data
        std::cout << "use_count: " << whoMadeCoffee[0].use_count() << std::endl;

        //Deleter
        std::shared_ptr<std::string> pNico5(new std::string("nico"), 
            [](std::string *p) {
            std::cout << "delete " << "*p" << std::endl;
            delete p;
        });

        pNico5 = nullptr;

        //Array
        std::shared_ptr<int> p1(new int[10],
            [](int *p) {
            delete[] p;
        });

        //可以将上面的改用这句完成
        std::shared_ptr<int> p2(new int[10], std::default_delete<int[]>());
    }

    class Person
    {
    public:
        std::string name;
        std::shared_ptr<Person> mother;
        std::shared_ptr<Person> father;

        //std::vector<std::shared_ptr<Person>> kids;          //使用share_ptr不会释放资源
        std::vector<std::weak_ptr<Person>> kids;              //使用weak_ptr才能释放资源

        Person(const std::string &n,
            std::shared_ptr<Person> m = nullptr,
            std::shared_ptr<Person> f = nullptr) : name(n), mother(m), father(f) {
            //nothing
        }

        ~Person()
        {
            std::cout << "delete " << name << std::endl;
        }
    };

    std::shared_ptr<Person> initFamily(const std::string &name)
    {
        std::shared_ptr<Person> mom(new Person(name + "'s mom"));
        std::shared_ptr<Person> dad(new Person(name + "'s dad"));
        std::shared_ptr<Person> kid(new Person(name, mom, dad));
        mom->kids.push_back(kid);
        dad->kids.push_back(kid);
        return kid;
    }

    void WeakPointer()
    {
        std::shared_ptr<Person> p = initFamily("nico");
        std::cout << "nico's family exists" << std::endl;
        std::cout << "- nico is shared " << p.use_count() << " times" << std::endl;
        //std::cout << "- name of 1st kid of nico's mom: " << p->mother->kids[0]->name << std::endl;            //share_ptr
        std::cout << "- name of 1st kid of nico's mom: " << p->mother->kids[0].lock()->name << std::endl;       //weak_ptr

        p = initFamily("jim");
        std::cout << "jim's family exists" << std::endl;
    }

    void UniquePointer()
    {
        std::unique_ptr<std::string> up(new std::string("nico"));
        (*up)[0] = 'N';
        up->append("lai");
        std::cout << *up << std::endl;

        //std::unique_ptr<std::string> up1 = new int;           //ERROR
        //std::unique_ptr<std::string> up2(new int(1));         //OK

        std::unique_ptr<std::string> up3;                       //empty
        up3 = nullptr;
        up3.reset();

        //release
        std::string *sp = up.release();                         //up lose ownership
        if (up) {                                               //if up is not empty
            std::cout << *up << std::endl;
        }

        std::unique_ptr<std::string> up4(new std::string("hello"));
        //std::unique_ptr<std::string> up5(up4);                //ERROR
        std::unique_ptr<std::string> up6(std::move(up4));       //OK

        //Array
        std::unique_ptr<std::string[]> up7(new std::string[10]);
        //std::cout << *up7 << std::endl;                       //ERROR
        std::cout << up7[0] << std::endl;                       //OK
    }
}

namespace C11_NumericLimit
{
    void Test()
    {
        //use textual representation for bool
        std::cout << std::boolalpha << std::endl;

        //print maximum
        std::cout << "max(short): " << std::numeric_limits<short>::max() << std::endl;
        std::cout << "max(int): " << std::numeric_limits<int>::max() << std::endl;
        std::cout << "max(long): " << std::numeric_limits<long>::max() << std::endl;
        std::cout << "max(float): " << std::numeric_limits<float>::max() << std::endl;
        std::cout << "max(double): " << std::numeric_limits<double>::max() << std::endl;
        std::cout << "max(long double): " << std::numeric_limits<long double>::max() << std::endl;

        //print whether char is signed
        std::cout << "is_signed(char): " << std::numeric_limits<char>::is_signed << std::endl;

        //print whether numeric limits for type string exist
        std::cout << "is_specialized(string): " << std::numeric_limits<std::string>::is_specialized << std::endl;
    }
}

namespace C11_TypeTraitAndTypeUtility
{
    template<typename T>
    void foo(const T &val)
    {
        if (std::is_pointer<T>::value)
        {
            std::cout << "foo() called for a pointer" << std::endl;
        }
        else
        {
            std::cout << "foo() called for a value" << std::endl;
        }
    }
}

namespace C11_Ratio
{
    void Test()
    {
        typedef std::ratio<5, 3> FiveThirds;
        std::cout << FiveThirds::num << "/" << FiveThirds::den << std::endl;

        typedef std::ratio<25, 15> AlsoFiveThirds;
        std::cout << AlsoFiveThirds::num << "/" << AlsoFiveThirds::den << std::endl;

        std::ratio<42, 42> one;
        std::cout << one.num << "/" << one.den << std::endl;

        std::ratio<0> zero;
        std::cout << zero.num << "/" << zero.den << std::endl;

        typedef std::ratio<7, -3> Neg;
        std::cout << Neg::num << "/" << Neg::den << std::endl;
    }
}

namespace C11_Chrono
{
    void TestDuration()
    {
        std::chrono::duration<int>                          twentySeconds(20);          //单位为秒
        std::chrono::duration<double, std::ratio<60>>       halfAMinute(0.5);           //单位为分钟
        std::chrono::duration<long, std::ratio<1, 1000>>    oneMillisecond(1);          //单位为毫秒

        //上面三行可以简写成如下
        std::chrono::seconds twentySeconds2(20);
        std::chrono::minutes oneMinute(1);
        std::chrono::milliseconds oneMillisecond2(1);
        std::chrono::hours aDay(24);

        //算术运算
        std::chrono::milliseconds ms;
        ms += twentySeconds2 + aDay;
        --ms;
        ms *= 2;
        std::cout << ms.count() << " ms" << std::endl;
        std::cout << std::chrono::nanoseconds(ms).count() << "ns" << std::endl;

        //精度转换
        std::chrono::seconds sec(55);
        //std::chrono::minutes m1 = sec;                                             //ERROR
        std::chrono::minutes m2 = std::chrono::duration_cast<std::chrono::minutes>(sec);
        std::cout << m2.count() << std::endl;

        //把毫秒转换为其它单位
        std::chrono::milliseconds ms2(7255042);
        std::chrono::hours hh = std::chrono::duration_cast<std::chrono::hours>(ms2);
        std::chrono::minutes mm = std::chrono::duration_cast<std::chrono::minutes>(ms2 % std::chrono::hours(1));
        std::chrono::seconds ss = std::chrono::duration_cast<std::chrono::seconds>(ms2 % std::chrono::minutes(1));
        std::chrono::milliseconds msec = std::chrono::duration_cast<std::chrono::milliseconds>(ms2 % std::chrono::seconds(1));
        std::cout << hh.count() << ":" << mm.count() << ":" << ss.count() << "." << msec.count() << std::endl;
    }

    template <typename C>
    void printClockData()
    {
        std::cout << "- precision: ";
        //if time unit is less or equal one millisecond
        typedef typename C::period P;   //type of time unit
        if (std::ratio_less_equal<P, std::milli>::value)
        {
            //convert to and print as milliseconds
            typedef typename std::ratio_multiply<P, std::kilo>::type TT;
            std::cout << std::fixed << double(TT::num) / TT::den << " milliseconds" << std::endl;
        }
        else
        {
            //print as seconds
            std::cout << std::fixed << double(P::num) / P::den << " seconds" << std::endl;
        }

        std::cout << "- is_steady: " << std::boolalpha << C::is_steady << std::endl;
    }

    void TestClock()
    {
        std::cout << "system_clock: " << std::endl;
        printClockData<std::chrono::system_clock>();
        std::cout << std::endl << "high_resolution_clock: " << std::endl;
        printClockData<std::chrono::high_resolution_clock>();
        std::cout << std::endl << "steady_clock: " << std::endl;
        printClockData<std::chrono::steady_clock>();
    }

    //convert timepoint of system clock to calendar time string
    std::string asString(const std::chrono::system_clock::time_point &tp)
    {
        std::time_t t = std::chrono::system_clock::to_time_t(tp);   //convert to system time
        std::string ts = std::ctime(&t);                            //convert to calendar time
        //std::string ts = std::asctime(gmtime(&t));                //UTC

        ts.resize(ts.size() - 1);                                   //skip trailing newline
        return ts;
    }

    //convert calendar time to timepoint of system clock
    std::chrono::system_clock::time_point makeTimePoint(int year, int mon, int day, int hour, int min, int sec = 0)
    {
        struct std::tm t;
        t.tm_sec = sec;
        t.tm_min = min;
        t.tm_hour = hour;
        t.tm_mday = day;
        t.tm_mon = mon - 1;
        t.tm_year = year - 1900;
        t.tm_isdst = -1;

        std::time_t tt = std::mktime(&t);
        if (tt == -1) {
            throw "no valid system time";
        }

        return std::chrono::system_clock::from_time_t(tt);
    }

    void TestTimepoint()
    {
        std::chrono::system_clock::time_point tp;
        std::cout << "epoch: " << asString(tp) << std::endl;

        tp = std::chrono::system_clock::now();
        std::cout << "now: " << asString(tp) << std::endl;

        //tp = std::chrono::system_clock::time_point::max();
        //std::cout << "max: " << asString(tp) << std::endl;

        //tp = std::chrono::system_clock::time_point::min();
        //std::cout << "min: " << asString(tp) << std::endl;

        auto tp1 = makeTimePoint(2010, 01, 01, 0, 0);
        std::cout << asString(tp1) << std::endl;
    }
}
