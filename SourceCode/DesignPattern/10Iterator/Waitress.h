#pragma once
#include <iostream>
#include "CafeMenu.h"
#include "DinerMenu.h"
#include "PancakeHouseMenu.h"

//Ů����Ա������˵�
class Waitress {
public:

    Waitress() {
    }

    //��ӡ�˵���ÿһ��
    void printMenu() {
        Iterator *pancakeIterator = m_pancakeHouseMenu.createIterator();
        Iterator *dinerIterator = m_dinerMenu.createIterator();
        Iterator *cafeIterator = m_cafeMenu.createIterator();
        std::cout << "MENU\n-----\nBREAKFAST" << std::endl;
        printMenu(pancakeIterator);
        std::cout << "\nLUNCH" << std::endl;
        printMenu(dinerIterator);
        std::cout << "\nCAFE" << std::endl;
        printMenu(cafeIterator);
    }

    //��ӡ���
    void printBreakfastMenu() {
    }

    //��ӡ���
    void printLunchMenu() {
    }

    //��ӡ��ʳ�˵�
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

    CafeMenu m_cafeMenu;
    DinerMenu m_dinerMenu;
    PancakeHouseMenu m_pancakeHouseMenu;
};



void TestIterator() {
    Waitress waitress;
    waitress.printMenu();
}
