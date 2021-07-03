#pragma once
#include "CMenu.h"
#include "CMenuItem.h"

//Ů����Ա
class CWaitress {
public:

    CWaitress(MenuComponent *allMenus) {
        m_allMenus = allMenus;
    }

    void printMenu() {
        m_allMenus->print();
    }

private:

    MenuComponent *m_allMenus;
};



void TestComposite() {
    //�ȴ������в˵�
    MenuComponent *pancakeHouseMenu = new CMenu("PANCAKE HOUSE MENU", "Breakfast");
    MenuComponent *dinerMenu = new CMenu("DINER MENU", "Lunch");
    MenuComponent *cafeMenu = new CMenu("CAFE MENU", "Dinner");
    MenuComponent *dessertMenu = new CMenu("DESSERT MENU", "Dessert of course!");

    //����һ�����Ĳ˵�
    MenuComponent *allMenus = new CMenu("ALL MENUS", "All menus combined");

    //���Ӳ˵����뵽����˵�
    allMenus->add(pancakeHouseMenu);
    allMenus->add(dinerMenu);
    allMenus->add(cafeMenu);

    //��Ӳ˵���
    dinerMenu->add(new CMenuItem("Pasta", "Spaghetti with Marinara Sauce, and a slice of sourdough bread", true, 3.89));
    dinerMenu->add(dessertMenu);

    dessertMenu->add(new CMenuItem("Apple Pie", "Apple pie with a flakey crust, topped with vanilla ice cream", true, 1.59));

    //��Ӹ���Ĳ˵���......

    //��ӡ�˵�
    CWaitress waitress(allMenus);
    waitress.printMenu();
}
