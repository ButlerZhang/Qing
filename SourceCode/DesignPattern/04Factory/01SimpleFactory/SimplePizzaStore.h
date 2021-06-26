#pragma once
#include "SimplePizza.h"
#include "SimplePizzaFactory.h"

class SimplePizzaStore {
public:

    SimplePizzaStore(SimplePizzaFactory *factory) {
        m_factory = factory;
    }

    SimplePizza *orderPizza(std::string type) {
        //披萨的类型由工厂决定
        SimplePizza *pizza = m_factory->createPizza(type);

        //制作披萨的步骤是固定的
        pizza->prepare();
        pizza->bake();
        pizza->cut();
        pizza->box();
        return pizza;
    }

private:

    SimplePizzaFactory *m_factory;
};
