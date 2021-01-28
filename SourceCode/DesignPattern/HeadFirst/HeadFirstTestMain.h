#pragma once
#include "StrategyPattern/Duck.h"


void headFirstTestMain() {
    Duck* mallard = new MallardDuck();
    mallard->performQuack();
    mallard->performFly();

    Duck* model = new ModelDuck();
    model->performFly();
    model->setFlyBehavior(new FlyRocketPowered());
    model->performFly();
}