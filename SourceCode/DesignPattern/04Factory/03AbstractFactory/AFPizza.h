#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "PizzaIngredientFactory.h"

class Dough;
class Sauce;
class Cheese;
class Veggies;
class Pepperoni;
class Clams;

class AFPizza {
public:

    AFPizza() {}
    virtual ~AFPizza() {}

    std::string getName() { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    virtual void prepare() = 0;

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
    Dough* m_dough;                         //面团
    Sauce* m_sauce;                         //酱料
    std::vector<Veggies*> m_veggies;        //蔬菜
    Cheese* m_cheese;
    Pepperoni* m_pepperoni;
    Clams* m_clam;
};

class ChessePizza : public AFPizza {
public:

    PizzaIngredientFactory* m_ingredientFactory;

    ChessePizza(PizzaIngredientFactory* ingredientFactory) {
        m_ingredientFactory = ingredientFactory;
    }

    virtual void prepare() {
        std::cout << "Preparing " << m_name << std::endl;
        m_dough = m_ingredientFactory->createDough();
        m_sauce = m_ingredientFactory->createSauce();
        m_cheese = m_ingredientFactory->createCheese();
    }
};

class ClamPizza : public AFPizza {
public:

    PizzaIngredientFactory* m_ingredientFactory;

    ClamPizza(PizzaIngredientFactory* ingredientFactory) {
        m_ingredientFactory = ingredientFactory;
    }

    virtual void prepare() {
        std::cout << "Preparing " << m_name << std::endl;
        m_dough = m_ingredientFactory->createDough();
        m_sauce = m_ingredientFactory->createSauce();
        m_cheese = m_ingredientFactory->createCheese();
        m_clam = m_ingredientFactory->createClams();
    }
};