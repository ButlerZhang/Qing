#pragma once
#include <list>
#include "MenuItem.h"
#include "Iterator.h"

class PancakeHouseMenu :public Menu {
public:

    PancakeHouseMenu() {
        m_menuItems.push_back(MenuItem("K&B's Pancake Breakfast",
                              "Pancakes with scrambled eggs, and toast",
                              true,
                              2.99));

        m_menuItems.push_back(MenuItem("Regular Pancake Breakfast",
                              "Pancakes with fried eggs, sausage",
                              false,
                              2.99));

        m_menuItems.push_back(MenuItem("Blueberry Pancakes",
                              "Pancakes make with fresh blueberries",
                              true,
                              3.49));

        m_menuItems.push_back(MenuItem("Waffles",
                              "Waffles, with your choice of blueberries or strawberries",
                              true,
                              3.59));
    }

    std::list<MenuItem> &getMenuItems() {
        return m_menuItems;
    }

    virtual Iterator *createIterator() {
        return new PancakeHouseIterator(m_menuItems);
    }

    //other methods

private:

    std::list<MenuItem> m_menuItems;
};
