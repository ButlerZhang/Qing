#pragma once
#include <vector>
#include <iostream>
#include "MenuComponent.h"

//菜单、子菜单
class CMenu : public MenuComponent {
public:

    CMenu(const std::string &name, const std::string &description) {
        m_name = name;
        m_description = description;
    }

    virtual void add(MenuComponent *menuComponent) {
        m_menuComponents.push_back(menuComponent);
    }

    virtual void remove(MenuComponent *menuComponent) {
        for (auto it = m_menuComponents.begin(); it != m_menuComponents.end(); it++) {
            if ((*it)->getGUID() == menuComponent->getGUID()) {
                m_menuComponents.erase(it);
                break;
            }
        }
    }

    virtual MenuComponent *getChild(int i) {
        return m_menuComponents[i];
    }

    virtual std::string getName() {
        return m_name;
    }

    virtual std::string getDescription() {
        return m_description;
    }

    virtual void print() {
        std::cout << "\n" << getName();
        std::cout << ",  " << getDescription() << std::endl;
        std::cout << "----------------------------" << std::endl;

        //打印子菜单时，组合模式使用了迭代器模式
        for (auto it = m_menuComponents.begin(); it != m_menuComponents.end(); it++) {
            (*it)->print();
        }
    }

private:

    std::string m_name;
    std::string m_description;
    std::vector<MenuComponent *> m_menuComponents;
};
