#pragma once
#include "SimplePizza.h"
#include "SimplePizzaFactory.h"

class SimplePizzaStore {
public:

    SimplePizzaStore(SimplePizzaFactory *factory) {
        m_factory = factory;
    }

    SimplePizza *orderPizza(std::string type) {
        //�����������ɹ�������
        SimplePizza *pizza = m_factory->createPizza(type);

        //���������Ĳ����ǹ̶���
        pizza->prepare();
        pizza->bake();
        pizza->cut();
        pizza->box();
        return pizza;
    }

private:

    SimplePizzaFactory *m_factory;
};
