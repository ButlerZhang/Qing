#pragma once
#include <string>
#include "Beverage.h"

class CondimentDecorator : public Beverage {
public:

    CondimentDecorator(Beverage* beverage) : m_beverage(beverage) {
    }

protected:

    Beverage* m_beverage;
};

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
