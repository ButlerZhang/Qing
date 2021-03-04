#pragma once
#include "Pizza.h"

class PizzaStore {
public:

    Pizza* orderPizza(std::string type) {
        Pizza* pizza = createPizza(type);
        pizza->prepare();
        pizza->bake();
        pizza->cut();
        pizza->box();
        return pizza;
    }

    virtual Pizza* createPizza(std::string type) = 0;
};

class NYStylePizzaStore : public PizzaStore {
public:
    virtual Pizza* createPizza(std::string type) {
        if (type == "cheese") {
            return new NYStyleChessePizza();
        }
        else if (type == "veggie") {
            //return new NYStyleVeggiePizza();
        }
        else if (type == "clam") {
            //return new NYStyleClamPizza();
        }
        else if (type == "pepperoni") {
            //return new NYStylePepperoniPizza();
        }

        return nullptr;
    }
};

class ChicagoPizzaStore : public PizzaStore {
public:
    virtual Pizza* createPizza(std::string type) {
        if (type == "cheese") {
            return new ChicagoStyleCheesePizza();
        }
        else if (type == "veggie") {
            //return new ChicagoVeggiePizza();
        }
        else if (type == "clam") {
            //return new ChicagoClamPizza();
        }
        else if (type == "pepperoni") {
            //return new ChicagoPepperoniPizza();
        }

        return nullptr;
    }
};
