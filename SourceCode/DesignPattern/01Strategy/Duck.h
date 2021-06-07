#pragma once
#include "FlyBehavior.h"
#include "QuackBehavior.h"

class Duck {
public:

    FlyBehavior *m_flyBehavior;
    QuackBehavior *m_quackBehavior;

    Duck() {
        m_flyBehavior = nullptr;
        m_quackBehavior = nullptr;
    }

    virtual ~Duck() {
    }

    virtual void display() = 0;

    void performFly() {
        m_flyBehavior->fly();
    }

    void performQuack() {
        m_quackBehavior->quack();
    }

    void swim() {
        std::cout << "All ducks float, even decoys!" << std::endl;
    }

    void setFlyBehavior(FlyBehavior *fb) {
        m_flyBehavior = fb;
    }

    void setQuackBehavior(QuackBehavior *qb) {
        m_quackBehavior = qb;
    }
};

class MallardDuck : public Duck {
public:

    MallardDuck() {
        //在构造函数里设定具体的行为
        m_quackBehavior = new Quack();
        m_flyBehavior = new FlyWithWings();
    }

    virtual void display() {
        std::cout << "I am a real Mallard duck." << std::endl;
    }
};

class ModelDuck : public Duck {
public:

    ModelDuck() {
        //在构造函数里提供默认行为，在运行时通过set函数改变默认行为
        m_quackBehavior = new Quack();
        m_flyBehavior = new FlyNoWay();
    }

    virtual void display() {
        std::cout << "I am a model duck" << std::endl;
    }
};



void TestStrategy() {
    Duck *mallard = new MallardDuck();
    mallard->performQuack();
    mallard->performFly();

    Duck *model = new ModelDuck();
    model->performFly();
    model->setFlyBehavior(new FlyRocketPowered());
    model->performFly();
}
