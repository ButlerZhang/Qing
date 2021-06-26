#pragma once
#include "Beverage.h"
#include "CondimentDecorator.h"
#include <iostream>

//四种咖啡，四种饮料

class HouseBlend : public Beverage {
public:

    HouseBlend() {
        m_description = "House Blend Coffee";
    }

    virtual double cost() {
        return 0.89;
    }
};

class DarkRoast : public Beverage {
public:

    DarkRoast() {
        m_description = "Dark Roast Coffee";
    }

    virtual double cost() {
        return 1.2;
    }
};

//浓缩咖啡
class Espresso : public Beverage {
public:

    Espresso() {
        m_description = "Espresso";
    }

    virtual double cost() {
        return 1.99;
    }
};

class Decaf : public Beverage {
public:

    Decaf() {
        m_description = "Decaf Coffee";
    }

    virtual double cost() {
        return 0.88;
    }
};



void TestDecorator() {
    //只定一杯Espresso，不需要调料
    Beverage *beverage = new Espresso();
    std::cout << beverage->getDescription() << " $" << beverage->cost() << std::endl;

    //制造一杯DarkRoast饮料，添加了两份Mocha
    Beverage *beverage2 = new DarkRoast();
    beverage2 = new Mocha(beverage2);
    beverage2 = new Mocha(beverage2);
    beverage2 = new Whip(beverage2);
    std::cout << beverage2->getDescription() << " $" << beverage2->cost() << std::endl;

    //制造一杯HouseBlend，添加了豆浆、摩卡和奶泡
    Beverage *beverage3 = new HouseBlend();
    beverage3 = new Soy(beverage3);
    beverage3 = new Mocha(beverage3);
    beverage3 = new Whip(beverage3);
    std::cout << beverage3->getDescription() << " $" << beverage3->cost() << std::endl;
}
