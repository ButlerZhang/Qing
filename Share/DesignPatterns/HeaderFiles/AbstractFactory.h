// Copyright (c) ButlerZhang 2016

#ifndef __ABSTRACT_FACTORY_H__
#define __ABSTRACT_FACTORY_H__



namespace QingTemplate
{
    class AbstractProductA
    {
    public:

        AbstractProductA() {}
        virtual ~AbstractProductA() {}
    };

    class AbstractProductA1 : public AbstractProductA
    {
    public:

        AbstractProductA1();
        virtual ~AbstractProductA1();
    };

    class AbstractProductA2 : public AbstractProductA
    {
    public:

        AbstractProductA2();
        virtual ~AbstractProductA2();
    };



    class AbstractProductB
    {
    public:

        AbstractProductB() {}
        virtual ~AbstractProductB() {}
    };

    class AbstractProductB1 : public AbstractProductB
    {
    public:

        AbstractProductB1();
        virtual ~AbstractProductB1();
    };

    class AbstractProductB2 : public AbstractProductB
    {
    public:

        AbstractProductB2();
        virtual ~AbstractProductB2();
    };



    class AbstractFactory
    {
    public:

        AbstractFactory(void) {}
        virtual ~AbstractFactory(void) {}

        virtual AbstractProductA* GetAbstractProductA() = 0;
        virtual AbstractProductB* GetAbstractProductB() = 0;
    };

    class AbstractFactory1 : public AbstractFactory
    {
    public:

        AbstractFactory1();
        virtual ~AbstractFactory1();

        virtual AbstractProductA* GetAbstractProductA();
        virtual AbstractProductB* GetAbstractProductB();
    };

    class AbstractFactory2 : public AbstractFactory
    {
    public:

        AbstractFactory2();
        virtual ~AbstractFactory2();

        virtual AbstractProductA* GetAbstractProductA();
        virtual AbstractProductB* GetAbstractProductB();
    };



    void AbstractFactoryTest();
}

#endif