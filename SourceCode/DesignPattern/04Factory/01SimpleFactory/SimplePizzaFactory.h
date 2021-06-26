#pragma once
#include "SimplePizza.h"

class SimplePizzaFactory {
public:

    //这个函数通常被声明为静态，可以避免创建工厂对象
    static SimplePizza *createPizza(std::string type) {
        if (type == "cheese") {
            return new CheesePizza();
        }
        else if (type == "pepperoni") {
            return new PepperoniPizza();
        }
        else if (type == "ClamPizza") {
            return new ClamPizza();
        }
        else if (type == "veggie") {
            return new VeggiePizza();
        }
        else {
            return nullptr;
        }
    }
};
