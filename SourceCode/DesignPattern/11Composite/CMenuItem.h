#pragma once
#include <iostream>
#include "MenuComponent.h"

//菜单项，叶子结点
class CMenuItem : public MenuComponent {
public:

    CMenuItem(const std::string &name,
              const std::string &description,
              bool isVegetarian,
              double price) {
        m_name = name;
        m_description = description;
        m_price = price;
        m_isVegetarian = isVegetarian;
    }

    virtual std::string getName() {
        return m_name;
    }

    virtual std::string getDescription() {
        return m_description;
    }

    virtual double getPrice() {
        return m_price;
    }

    virtual bool isVegetarian() {
        return m_isVegetarian;
    }

    virtual void print() {
        std::cout << " " << getName();
        if (isVegetarian()) {
            std::cout << "(v)";
        }
        std::cout << ", " << getPrice() << std::endl;
        std::cout << "     -----" << getDescription() << std::endl;
    }

private:

    std::string m_name;
    std::string m_description;
    double      m_price;
    bool        m_isVegetarian;
};
