#pragma once
#include <string>
#include "Beverage.h"

//调料装饰者基类，必须继承自饮料抽象基类
class CondimentDecorator : public Beverage {
public:

    CondimentDecorator(Beverage* beverage) : m_beverage(beverage) {
    }

protected:

    Beverage* m_beverage;
};

//奶泡是一个装饰者
class Milk : public CondimentDecorator {
public:

    Milk(Beverage* beverage) : CondimentDecorator(beverage) {
    }

    virtual double cost() {
        return 0.1 + m_beverage->cost();
    }

    virtual std::string getDescription() {
        return m_beverage->getDescription() + ", Milk";
    }
};

//摩卡是一个装饰者
class Mocha : public CondimentDecorator {
public:

    Mocha(Beverage* beverage) : CondimentDecorator(beverage) {
    }

    virtual double cost() {
        return 0.20 + m_beverage->cost();
    }

    virtual std::string getDescription() {
        return m_beverage->getDescription() + ", Mocha";
    }
};

//豆浆是一个装饰者
class Soy : public CondimentDecorator {
public:

    Soy(Beverage* beverage) : CondimentDecorator(beverage) {
    }

    virtual double cost() {
        double cost = m_beverage->cost();
        if (getSize() == CP_TALL) {
            cost += 0.10;
        }
        else if (getSize() == CP_GRANDE) {
            cost += 0.15;
        }
        else if (getSize() == CP_VENTI) {
            cost += 0.20;
        }

        return cost;
    }

    virtual std::string getDescription() {
        return m_beverage->getDescription() + ", Soy";
    }
};

class Whip : public CondimentDecorator {
public:

    Whip(Beverage* beverage) : CondimentDecorator(beverage) {
    }

    virtual double cost() {
        return 0.4 + m_beverage->cost();
    }

    virtual std::string getDescription() {
        return m_beverage->getDescription() + ", Whip";
    }
};
