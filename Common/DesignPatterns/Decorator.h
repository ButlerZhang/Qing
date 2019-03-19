#pragma once



class Component
{
public:

    virtual ~Component();
    virtual void Operation() = 0;

protected:

    Component() {}
};

class ConcreteComponent : public Component
{
public:

    ConcreteComponent() {}
    virtual ~ConcreteComponent();
    virtual void Operation();
};



class Decorator : public Component
{
public:

    Decorator(Component *pCom);
    virtual ~Decorator();
    virtual void Operation();

    void SetComponent(Component *pCom);

protected:

    Component *m_pComponent;
};

class ConcreteDecoratorA : public Decorator
{
public:

    ConcreteDecoratorA(Component *pCom) : Decorator(pCom) { }
    virtual ~ConcreteDecoratorA();
    virtual void Operation();

private:

    void AddBehavorA();
};

class ConcreteDecoratorB : public Decorator
{
public:

    ConcreteDecoratorB(Component *pCom) : Decorator(pCom) { }
    virtual ~ConcreteDecoratorB();
    virtual void Operation();

private:

    void AddBehavorB();
};

class ConcreteDecoratorC : public Decorator
{
public:

    ConcreteDecoratorC(Component *pCom) : Decorator(pCom) { }
    virtual ~ConcreteDecoratorC();
    virtual void Operation();

private:

    void AddBehavorC();
};

class ConcreteDecoratorD : public Decorator
{
public:

    ConcreteDecoratorD(Component *pCom) : Decorator(pCom) { }
    virtual ~ConcreteDecoratorD();
    virtual void Operation();

private:

    void AddBehavorD();
};



void TestDecorator();
