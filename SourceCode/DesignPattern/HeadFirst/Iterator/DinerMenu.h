#pragma once
#include <vector>
#include "MenuItem.h"
#include "Iterator.h"

class DinerMenu : public Menu {
public:

    DinerMenu() {
        m_menuItems.push_back(MenuItem("Vegetarian BLT",
                              "(Fakin') Bacon with lettuce & tomato on whole wheat",
                              true,
                              2.99));

        m_menuItems.push_back(MenuItem("BLT",
                              "Bacon with lettuce & tomato on whole wheat",
                              false,
                              2.99));

        m_menuItems.push_back(MenuItem("Soup of the day",
                              "Soup of the day, with a side of potato salad",
                              false,
                              3.49));

        m_menuItems.push_back(MenuItem("Hotdog",
                              "A hot dog, with saurkraut, relish, onions, topped with cheese",
                              false,
                              3.05));
    }

    std::vector<MenuItem> &getMenuItems() {
        return m_menuItems;
    }

    virtual Iterator *createIterator() {
        return new DinerMenuIterator(m_menuItems);
    }

    //other methods

private:

    std::vector<MenuItem> m_menuItems;
};

