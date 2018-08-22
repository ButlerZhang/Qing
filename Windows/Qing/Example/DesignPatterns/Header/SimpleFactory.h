// Copyright (c) ButlerZhang 2016

#ifndef __SIMPLE_FACTORY_H__
#define __SIMPLE_FACTORY_H__



enum SimpleProductType
{
    SPT_A = 0,
    SPT_B,
    SPT_C,
    SPT_D,

    SPT_COUNT
};

class SimpleProduct
{
public:

    SimpleProduct() {}
    virtual ~SimpleProduct() {}
};

class SimpleProductA : public SimpleProduct
{
public:

    SimpleProductA();
    virtual ~SimpleProductA();
};

class SimpleProductB : public SimpleProduct
{
public:

    SimpleProductB();
    virtual ~SimpleProductB();
};

//SimpleProductC, SimpleProductD, ...



class SimpleFactory
{
public:

    SimpleFactory() {}
    ~SimpleFactory() {}
    SimpleProduct* GetSimpleProduct(SimpleProductType ProductType);
};



void SimpleFactoryTest();

#endif