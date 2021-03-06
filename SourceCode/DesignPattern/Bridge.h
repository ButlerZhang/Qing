#pragma once



class Implementor
{
public:

    Implementor() {}
    virtual ~Implementor() {}

    virtual void OperatorImpl() = 0;
};

class ConcreteImplementorA : public Implementor
{
public:

    ConcreteImplementorA() {}
    virtual ~ConcreteImplementorA() {}

    virtual void OperatorImpl();
};

class ConcreteImplementorB : public Implementor
{
public:

    ConcreteImplementorB() {}
    virtual ~ConcreteImplementorB() {}

    virtual void OperatorImpl();
};



class Abstraction
{
public:

    Abstraction(Implementor *pImplementor) {}
    virtual ~Abstraction() {}

    virtual void Operator() = 0;
};

class RefineAbstractionA : public Abstraction
{
public:

    RefineAbstractionA(Implementor *pImplementor);
    virtual ~RefineAbstractionA();

    virtual void Operator();

private:

    Implementor *m_Implementor;
};

class RefineAbstractionB : public Abstraction
{
public:

    RefineAbstractionB(Implementor *pImplementor);
    virtual ~RefineAbstractionB();

    virtual void Operator();

private:

    Implementor *m_Implementor;
};



void TestBridge();
