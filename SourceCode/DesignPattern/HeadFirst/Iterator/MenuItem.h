#pragma once
#include <string>

class MenuItem {
public:
    MenuItem(const std::string &name,
             const std::string &description,
             bool vegetarian,
             double price) {
        m_name = name;
        m_price = price;
        m_vegetarian = vegetarian;
        m_description = description;
    }

    const std::string &getName() const {
        return m_name;
    }

    const std::string &getDescription() const {
        return m_description;
    }

    double getPrice() const {
        return m_price;
    }

    bool isVegetarian() const {
        return m_vegetarian;
    }

private:

    bool m_vegetarian;
    double m_price;
    std::string m_name;
    std::string m_description;
};
