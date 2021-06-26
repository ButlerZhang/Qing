#pragma once
#include <vector>
#include <string>
#include <iostream>

//简单披萨基类
class SimplePizza {
public:

    SimplePizza() {}
    virtual ~SimplePizza() {}

    std::string getName() { return m_name; }

    virtual void prepare() {
        std::cout << "Preparing " << m_name << std::endl;
        std::cout << "Tossing dough..." << std::endl;
        std::cout << "Adding sauce..." << std::endl;
        std::cout << "Adding toppings:" << std::endl;
        for (std::vector<int>::size_type index = 0; index < m_toppings.size(); index++) {
            std::cout << "    " << m_toppings[index] << std::endl;
        }
    }

    virtual void bake() {
        std::cout << "Bake for 25 minutes at 350" << std::endl;
    }

    virtual void cut() {
        std::cout << "Cutting the pizza into diagonal slices" << std::endl;
    }

    virtual void box() {
        std::cout << "Place pizza in offical PizzaStore box" << std::endl;
    }

protected:

    std::string m_name;                     //名称
    std::string m_dough;                    //面团
    std::string m_sauce;                    //酱料
    std::vector<std::string> m_toppings;    //佐料
};

class CheesePizza : public SimplePizza {
public:
    CheesePizza() {
        m_name = "cheese";
    }
};

class PepperoniPizza : public SimplePizza {
public:
    PepperoniPizza() {
        m_name = "pepperoni";
    }
};

class ClamPizza : public SimplePizza {
public:
    ClamPizza() {
        m_name = "ClamPizza";
    }
};

class VeggiePizza : public SimplePizza {
public:
    VeggiePizza() {
        m_name = "veggie";
    }
};
