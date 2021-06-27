#pragma once
#include "SimplePizza.h"

class SimplePizzaFactory {
public:

    //�������ͨ��������Ϊ��̬�����Ա��ⴴ����������
    //������û��ʹ�þ�̬��������ʾ��
    SimplePizza *createPizza(std::string type) {
        if (type == "cheese") {
            return new CheesePizza();
        }
        else if (type == "pepperoni") {
            return new PepperoniPizza();
        }
        else if (type == "ClamPizza") {
            return new ClamPizza();
        }
        else if (type == "veggie") {
            return new VeggiePizza();
        }
        else {
            return nullptr;
        }
    }
};
