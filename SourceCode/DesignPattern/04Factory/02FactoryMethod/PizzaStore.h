#pragma once
#include "Pizza.h"

//�����̵�
class PizzaStore {
public:

    Pizza* orderPizza(std::string type) {
        //�����������ɹ˿;���
        Pizza* pizza = createPizza(type);

        //���������Ĳ����ǹ̶���
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
