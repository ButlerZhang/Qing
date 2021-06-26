#pragma once
#include "Pizza.h"

//披萨商店
class PizzaStore {
public:

    Pizza* orderPizza(std::string type) {
        //披萨的类型由顾客决定
        Pizza* pizza = createPizza(type);

        //制作披萨的步骤是固定的
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



void TestFactoryMethod() {
    PizzaStore *nyStore = new NYStylePizzaStore();
    PizzaStore *chicagoStore = new ChicagoPizzaStore();

    Pizza *pizza = nyStore->orderPizza("cheese");
    std::cout << "Ethan ordered a " << pizza->getName() << std::endl << std::endl;

    pizza = chicagoStore->orderPizza("cheese");
    std::cout << "Joel ordered a " << pizza->getName() << std::endl << std::endl;
}
