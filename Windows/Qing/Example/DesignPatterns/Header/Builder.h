#pragma once
#include "..\..\..\HeaderFiles\QingBase.h"

QING_TEMPLATE_BEGIN



class Builder
{
public:

    Builder(void) {}
    virtual ~Builder(void) {}

    virtual void BuilderPartOne() = 0;
    virtual void BuilderPartTwo() = 0;
};

class BuilderA : public Builder
{
public:

    BuilderA() {}
    virtual ~BuilderA() {}

    virtual void BuilderPartOne();
    virtual void BuilderPartTwo();
};

class BuilderB : public Builder
{
public:

    BuilderB() {}
    virtual ~BuilderB() {}

    virtual void BuilderPartOne();
    virtual void BuilderPartTwo();
};



class Director
{
public:

    Director(Builder *pBuilder);
    ~Director();

    void Construct();

private:

    Builder *m_pBuilder;
};



void QING_DLL BuilderTest();

QING_TEMPLATE_END