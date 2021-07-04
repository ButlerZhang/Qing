#pragma once
#include "CDuck.h"

//抽象工厂模式
class AbstractDuckFactory {
public:
    virtual Quackable *createMallardDuck() = 0;
    virtual Quackable *createRedHeadDuck() = 0;
    virtual Quackable *createDuckCall() = 0;
    virtual Quackable *createRubberDuck() = 0;
};

//创建鸭子的工厂
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

//统计鸭子叫声的工厂
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
