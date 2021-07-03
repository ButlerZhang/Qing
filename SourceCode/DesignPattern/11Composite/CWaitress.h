#pragma once
#include "CMenu.h"
#include "CMenuItem.h"

//女服务员
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
    //先创建所有菜单
    MenuComponent *pancakeHouseMenu = new CMenu("PANCAKE HOUSE MENU", "Breakfast");
    MenuComponent *dinerMenu = new CMenu("DINER MENU", "Lunch");
    MenuComponent *cafeMenu = new CMenu("CAFE MENU", "Dinner");
    MenuComponent *dessertMenu = new CMenu("DESSERT MENU", "Dessert of course!");

    //创建一个最顶层的菜单
    MenuComponent *allMenus = new CMenu("ALL MENUS", "All menus combined");

    //把子菜单加入到顶层菜单
    allMenus->add(pancakeHouseMenu);
    allMenus->add(dinerMenu);
    allMenus->add(cafeMenu);

    //添加菜单项
    dinerMenu->add(new CMenuItem("Pasta", "Spaghetti with Marinara Sauce, and a slice of sourdough bread", true, 3.89));
    dinerMenu->add(dessertMenu);

    dessertMenu->add(new CMenuItem("Apple Pie", "Apple pie with a flakey crust, topped with vanilla ice cream", true, 1.59));

    //添加更多的菜单项......

    //打印菜单
    CWaitress waitress(allMenus);
    waitress.printMenu();
}
