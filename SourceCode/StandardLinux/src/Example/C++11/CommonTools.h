#pragma once
#include <tuple>
#include <ostream>
#include <iostream>
#include <complex>
#include <memory>
#include <vector>
#include <string>



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

namespace C11_SmartPointer
{
    void SharedPointer()
    {
        //two shared pointers representing two persons by their name
        std::shared_ptr<std::string> pNico(new std::string("nico"));
        std::shared_ptr<std::string> pJutta(new std::string("jutta"));

        //������ʼ������
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

        //���Խ�����ĸ���������
        std::shared_ptr<int> p2(new int[10], std::default_delete<int[]>());
    }

    class Person
    {
    public:
        std::string name;
        std::shared_ptr<Person> mother;
        std::shared_ptr<Person> father;

        //std::vector<std::shared_ptr<Person>> kids;          //ʹ��share_ptr�����ͷ���Դ
        std::vector<std::weak_ptr<Person>> kids;              //ʹ��weak_ptr�����ͷ���Դ

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
}
