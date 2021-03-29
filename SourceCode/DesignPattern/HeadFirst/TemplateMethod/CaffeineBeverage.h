#pragma once
#include <iostream>

class CaffeineBeverage {
public:

    //模板方法模式
    //好莱坞原则：别调用(打电话给)我们，我们会调用(打电话给)你
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

    //钩子函数
    //可以用来决定某些方法怎么执行，即修改模板方法的调用流程
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
