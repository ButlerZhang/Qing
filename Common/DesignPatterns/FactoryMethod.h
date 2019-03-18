#pragma once



class MethodProduct
{
public:

    MethodProduct() {}
    virtual ~MethodProduct() {}
};

class MethodProductA : public MethodProduct
{
public:

    MethodProductA();
    virtual ~MethodProductA();
};

class MethodProductB : public MethodProduct
{
public:

    MethodProductB();
    virtual ~MethodProductB();
};

//MethodProductC, MethodProductD, ...



class FactoryMethod
{
public:

    FactoryMethod(void) {}
    virtual ~FactoryMethod(void) {}
    virtual MethodProduct* GetMethodProduct() = 0;
};

class FactoryMethodA : public FactoryMethod
{
public:

    FactoryMethodA();
    virtual ~FactoryMethodA();
    virtual MethodProduct* GetMethodProduct();
};

class FactoryMethodB : public FactoryMethod
{
public:

    FactoryMethodB();
    virtual ~FactoryMethodB();
    virtual MethodProduct* GetMethodProduct();
};

//FactoryMethodC, FactoryMethodD, ...



void QING_DLL FactoryMethodTest();
