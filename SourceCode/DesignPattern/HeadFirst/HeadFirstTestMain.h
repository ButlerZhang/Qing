#pragma once
#include "Strategy/Duck.h"
#include "Observer/WeatherData.h"
#include "Observer/DisplayElement.h"
#include "Decorator/CoffeeComponent.h"
#include "Decorator/CondimentDecorator.h"

void TestStrategy() {
    Duck* mallard = new MallardDuck();
    mallard->performQuack();
    mallard->performFly();

    Duck* model = new ModelDuck();
    model->performFly();
    model->setFlyBehavior(new FlyRocketPowered());
    model->performFly();
}

void TestObserver() {
    WeatherData weatherData;
    CurrentConfitionDisplay currentDiisplay(&weatherData);
    StatisticsDisplay statisticsDisplay(&weatherData);
    ForecastDisplay forecastDisplay(&weatherData);

    weatherData.setMeasurements(80, 65, 30.4f);
    weatherData.setMeasurements(82, 70, 29.2f);
    weatherData.setMeasurements(78, 90, 29.2f);
}

void TestDecorator() {
    //只定一杯Espresso，不需要调料
    Beverage* beverage = new Espresso();
    std::cout << beverage->getDescription() << " $" << beverage->cost() << std::endl;

    //制造一杯DarkRoast饮料，添加了两份Mocha
    Beverage* beverage2 = new DarkRoast();
    beverage2 = new Mocha(beverage2);
    beverage2 = new Mocha(beverage2);
    beverage2 = new Whip(beverage2);
    std::cout << beverage2->getDescription() << " $" << beverage2->cost() << std::endl;

    //制造一杯HouseBlend，添加了豆浆、摩卡和奶泡
    Beverage* beverage3 = new HouseBlend();
    beverage3 = new Soy(beverage3);
    beverage3 = new Mocha(beverage3);
    beverage3 = new Whip(beverage3);
    std::cout << beverage3->getDescription() << " $" << beverage3->cost() << std::endl;
}

void headFirstTestMain() {
    //TestStrategy();
    //TestObserver();
    TestDecorator();
}
