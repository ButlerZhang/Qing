#pragma once
#include "..\..\..\HeaderFiles\QingBase.h"

QING_TEMPLATE_BEGIN



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



void QING_DLL SimpleFactoryTest();

QING_TEMPLATE_END