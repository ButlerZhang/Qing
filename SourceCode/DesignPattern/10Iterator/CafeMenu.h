#pragma once
#include "MenuItem.h"
#include "Iterator.h"

class CafeMenu : public Menu {
public:

    CafeMenu() {
        MenuItem mt1("Vgegie Burger and Air Fries",
                     "Veggie burger on a whole wheat bun, lettuce, tomato, and fries",
                     true,
                     3.99);
        m_menuItems.insert(std::pair<int, MenuItem>(0, mt1));

        MenuItem mt2("Soup of the day",
                     "A cup of the soup of the day, with a side salad",
                     false,
                     3.69);
        m_menuItems.insert(std::pair<int, MenuItem>(1, mt2));

        MenuItem mt3("Burrito",
                     "a large burrito, with whole pinto beans, salsa, guacamole",
                     true,
                     4.49);
        m_menuItems.insert(std::pair<int, MenuItem>(2, mt3));
    }

    virtual Iterator *createIterator() {
        return new CafeIterator(m_menuItems);
    }

private:

    std::map<int, MenuItem> m_menuItems;
};
