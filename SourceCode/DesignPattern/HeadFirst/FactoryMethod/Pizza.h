#pragma once
#include <vector>
#include <string>
#include <iostream>

class Pizza {
public:

    Pizza() {}
    virtual ~Pizza() {}

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

class NYStyleChessePizza : public Pizza {
public:
    NYStyleChessePizza() {
        m_name = "NY style Sauce and Cheese Pizza";
        m_dough = "Thin Crust Dough";
        m_sauce = "Marinara Sauce";
        m_toppings.push_back("Grated Reggiano Cheese");
    }
};

class ChicagoStyleCheesePizza : public Pizza {
public:
    ChicagoStyleCheesePizza() {
        m_name = "Chicago Style Deep Dish Cheese Pizza";
        m_dough = "Extra Thick Crust Dough";
        m_sauce = "Plum Tomato Sauce";
        m_toppings.push_back("Sharedded Mozzarella Cheese");
    }

    virtual void cut() {
        std::cout << "Cutting the pizza into square slices" << std::endl;
    }
};
