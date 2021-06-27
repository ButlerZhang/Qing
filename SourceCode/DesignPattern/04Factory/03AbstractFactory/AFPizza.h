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

    AFPizza() {
        m_dough = nullptr;
        m_sauce = nullptr;
        m_cheese = nullptr;
        m_pepperoni = nullptr;
        m_clam = nullptr;
    }

    virtual ~AFPizza() {}

    std::string getName() { return m_name; }
    void setName(const std::string &name) { m_name = name; }

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

    std::string m_name;                     //Ãû³Æ
    Dough *m_dough;                         //ÃæÍÅ
    Sauce *m_sauce;                         //½´ÁÏ
    Clams *m_clam;                          //¹þÀ®
    Cheese *m_cheese;                       //Ö¥Ê¿
    Pepperoni *m_pepperoni;                 //Ïã³¦
    std::vector<Veggies *> m_veggies;       //Êß²Ë
};

class AFChessePizza : public AFPizza {
public:

    PizzaIngredientFactory *m_ingredientFactory;

    AFChessePizza(PizzaIngredientFactory *ingredientFactory) {
        m_ingredientFactory = ingredientFactory;
    }

    virtual void prepare() {
        std::cout << "Preparing " << m_name << std::endl;
        m_dough = m_ingredientFactory->createDough();
        m_sauce = m_ingredientFactory->createSauce();
        m_cheese = m_ingredientFactory->createCheese();
    }
};

class AFClamPizza : public AFPizza {
public:

    PizzaIngredientFactory *m_ingredientFactory;

    AFClamPizza(PizzaIngredientFactory *ingredientFactory) {
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
