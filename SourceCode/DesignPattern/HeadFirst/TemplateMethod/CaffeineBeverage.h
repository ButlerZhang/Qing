#pragma once
#include <iostream>

class CaffeineBeverage {
public:

    //ģ�巽��ģʽ
    //������ԭ�򣺱����(��绰��)���ǣ����ǻ����(��绰��)��
    void prepareRecipe() {
        boilWater();
        brew();
        pourInCup();

        if (customerWantsCondiments()) {
            addCondiments();
        }
    }

    virtual void brew() = 0;
    virtual void addCondiments() = 0;

    void boilWater() {
        std::cout << "Boiling water" << std::endl;
    }

    void pourInCup() {
        std::cout << "Pouring into cup" << std::endl;
    }

    //���Ӻ���
    //������������ĳЩ������ôִ�У����޸�ģ�巽���ĵ�������
    virtual bool customerWantsCondiments() {
        return true;
    }
};

class Tea : public CaffeineBeverage {
public:

    virtual void brew() {
        std::cout << "Steeping the tea" << std::endl;
    }

    virtual void addCondiments() {
        std::cout << "Adding Lemon" << std::endl;
    }
};

class Coffee : public CaffeineBeverage {
public:

    virtual void brew() {
        std::cout << "Dripping Coffee through filter" << std::endl;
    }

    virtual void addCondiments() {
        std::cout << "Adding Sugar and Milk" << std::endl;
    }
};

class CoffeeWithHook : public CaffeineBeverage {
public:

    virtual void brew() {
        std::cout << "Dripping Coffee through filter" << std::endl;
    }

    virtual void addCondiments() {
        std::cout << "Adding Sugar and Milk" << std::endl;
    }

    virtual bool customerWantsCondiments() {
        std::string answer;
        std::cin >> answer;
        return (answer == "Y" || answer == "y");
    }
};
