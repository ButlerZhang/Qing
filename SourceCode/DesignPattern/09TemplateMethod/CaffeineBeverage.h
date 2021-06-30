#pragma once
#include <iostream>

//咖啡因饮料
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

    //酿造
    virtual void brew() = 0;

    //添加调味品
    virtual void addCondiments() = 0;

    //把水煮沸
    void boilWater() {
        std::cout << "Boiling water" << std::endl;
    }

    //倒进杯子
    void pourInCup() {
        std::cout << "Pouring into cup" << std::endl;
    }

    //钩子函数
    //可以用来决定某些方法怎么执行，即修改模板方法的调用流程
    virtual bool customerWantsCondiments() {
        return true;
    }
};

//茶
class Tea : public CaffeineBeverage {
public:

    virtual void brew() {
        std::cout << "Steeping the tea" << std::endl;
    }

    virtual void addCondiments() {
        std::cout << "Adding Lemon" << std::endl;
    }
};

//咖啡
class Coffee : public CaffeineBeverage {
public:

    virtual void brew() {
        std::cout << "Dripping Coffee through filter" << std::endl;
    }

    virtual void addCondiments() {
        std::cout << "Adding Sugar and Milk" << std::endl;
    }
};

//根据Hook判断是否添加饮料
class CoffeeWithHook : public CaffeineBeverage {
public:

    virtual void brew() {
        std::cout << "Dripping Coffee through filter" << std::endl;
    }

    virtual void addCondiments() {
        std::cout << "Adding Sugar and Milk" << std::endl;
    }

    //由客户决定是否添加调料
    virtual bool customerWantsCondiments() {
        std::string answer;
        std::cin >> answer;
        return (answer == "Y" || answer == "y");
    }
};



void TestTemplateMethod() {
    Tea tea;
    tea.prepareRecipe();

    Coffee coffee;
    coffee.prepareRecipe();
}
