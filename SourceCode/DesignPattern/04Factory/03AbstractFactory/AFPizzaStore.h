#pragma once
#include "AFPizza.h"

class AFPizzaStore {
public:

    AFPizza* orderPizza(std::string type) {
        AFPizza* pizza = createPizza(type);
        pizza->prepare();
        pizza->bake();
        pizza->cut();
        pizza->box();
        return pizza;
    }

    virtual AFPizza* createPizza(std::string type) = 0;
};

class NYStylePizzaStore : public AFPizzaStore {
public:
    virtual AFPizza* createPizza(std::string type) {
        AFPizza* pizza = nullptr;
        PizzaIngredientFactory* ingredientFactory = new NYPizzaIngredientFactory();

        if (type == "cheese") {
            pizza = new ChessePizza(ingredientFactory);
            pizza->setName("New York Style Cheese Pizza");
        }
        else if (type == "veggie") {
            //pizza = new VeggiePizza(ingredientFactory);
            //pizza->setName("New York Style Veggie Pizza");
        }
        else if (type == "clam") {
            pizza = new ClamPizza(ingredientFactory);
            pizza->setName("New York Style Clam Pizza");
        }
        else if (type == "pepperoni") {
            //pizza = new PepperoniPizza(ingredientFactory);
            //pizza->setName("New York Style Pepperoni Pizza");
        }

        return pizza;
    }
};

class ChicagoPizzaStore : public AFPizzaStore {
public:
    virtual AFPizza* createPizza(std::string type) {
        AFPizza* pizza = nullptr;
        PizzaIngredientFactory* ingredientFactory = new NYPizzaIngredientFactory();

        if (type == "cheese") {
            pizza = new ChessePizza(ingredientFactory);
            pizza->setName("Chicago Style Cheese Pizza");
        }
        else if (type == "veggie") {
            //pizza = new VeggiePizza(ingredientFactory);
            //pizza->setName("Chicago Style Veggie Pizza");
        }
        else if (type == "clam") {
            pizza = new ClamPizza(ingredientFactory);
            pizza->setName("Chicago Style Clam Pizza");
        }
        else if (type == "pepperoni") {
            //pizza = new PepperoniPizza(ingredientFactory);
            //pizza->setName("Chicago Style Pepperoni Pizza");
        }

        return pizza;
    }
};
