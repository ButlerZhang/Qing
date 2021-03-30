#pragma once
#include <iostream>
#include "DinerMenu.h"
#include "PancakeHouseMenu.h"

class Waitress {
public:

    Waitress() {
    }

    void printMenu() {
        Iterator *pancakeIterator = m_pancakeHouseMenu.createIterator();
        Iterator *dinerIterator = m_dinerMenu.createIterator();
        std::cout << "MENU\n-----\nBREAKFAST" << std::endl;
        printMenu(pancakeIterator);
        std::cout << "\nLUNCH" << std::endl;
        printMenu(dinerIterator);
    }

    void printBreakfastMenu() {
    }

    void printLunchMenu() {
    }

    void printVegetarianMenu() {
    }

private:

    void printMenu(Iterator *iterator) {
        while (iterator->hasNext()) {
            MenuItem *menuItem = iterator->next();
            std::cout << menuItem->getName() << ","
                << menuItem->getPrice() << " -- "
                << menuItem->getDescription() << std::endl;
        }
    }

private:

    DinerMenu m_dinerMenu;
    PancakeHouseMenu m_pancakeHouseMenu;
};
