#pragma once
#include "CDuck.h"

//���󹤳�ģʽ
class AbstractDuckFactory {
public:
    virtual Quackable *createMallardDuck() = 0;
    virtual Quackable *createRedHeadDuck() = 0;
    virtual Quackable *createDuckCall() = 0;
    virtual Quackable *createRubberDuck() = 0;
};

//����Ѽ�ӵĹ���
class DuckFactory : public AbstractDuckFactory {
public:
    virtual Quackable *createMallardDuck() {
        return new CMallardDuck();
    }

    virtual Quackable *createRedHeadDuck() {
        return new CRedHeadDuck();
    }

    virtual Quackable *createDuckCall() {
        return new CDuckCall();
    }

    virtual Quackable *createRubberDuck() {
        return new CRubberDuck();
    }
};

//ͳ��Ѽ�ӽ����Ĺ���
class CountingDuckFactory : public AbstractDuckFactory {
public:
    virtual Quackable *createMallardDuck() {
        return new QuackCounter(new CMallardDuck());
    }

    virtual Quackable *createRedHeadDuck() {
        return new QuackCounter(new CRedHeadDuck());
    }

    virtual Quackable *createDuckCall() {
        return new QuackCounter(new CDuckCall());
    }

    virtual Quackable *createRubberDuck() {
        return new QuackCounter(new CRubberDuck());
    }
};
