#pragma once
#include "Beverage.h"
#include "CondimentDecorator.h"
#include <iostream>

//���ֿ��ȣ���������

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

//Ũ������
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
    //ֻ��һ��Espresso������Ҫ����
    Beverage *beverage = new Espresso();
    std::cout << beverage->getDescription() << " $" << beverage->cost() << std::endl;

    //����һ��DarkRoast���ϣ����������Mocha
    Beverage *beverage2 = new DarkRoast();
    beverage2 = new Mocha(beverage2);
    beverage2 = new Mocha(beverage2);
    beverage2 = new Whip(beverage2);
    std::cout << beverage2->getDescription() << " $" << beverage2->cost() << std::endl;

    //����һ��HouseBlend������˶�����Ħ��������
    Beverage *beverage3 = new HouseBlend();
    beverage3 = new Soy(beverage3);
    beverage3 = new Mocha(beverage3);
    beverage3 = new Whip(beverage3);
    std::cout << beverage3->getDescription() << " $" << beverage3->cost() << std::endl;
}
