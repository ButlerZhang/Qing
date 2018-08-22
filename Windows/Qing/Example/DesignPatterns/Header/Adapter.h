// Copyright (c) ButlerZhang 2016

#ifndef __ADAPTER_H__
#define __ADAPTER_H__



class Target
{
public:

    Target() {}
    virtual ~Target() {}

    virtual void Request();
};



class Adaptee
{
public:

    Adaptee() {}
    virtual ~Adaptee() {}

    virtual void SpecificRequest();
};



class ClassAdapter : public Target, private Adaptee
{
public:

    ClassAdapter() {}
    virtual ~ClassAdapter() {}

    virtual void Request();
};



class ObjectAdapter : public Target
{
public:

    ObjectAdapter();
    virtual ~ObjectAdapter();

    virtual void Request();

private:

    Adaptee *m_pAdaptee;
};



void TestAdapter();

#endif