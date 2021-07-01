#pragma once
#include <string>

//菜单项
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

    bool m_vegetarian;              //是否是素食主义者
    double m_price;                 //价格
    std::string m_name;             //菜名
    std::string m_description;      //描述
};
