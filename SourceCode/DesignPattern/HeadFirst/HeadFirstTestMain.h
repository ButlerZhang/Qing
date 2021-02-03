#pragma once
#include "Strategy/Duck.h"
#include "Observer/WeatherData.h"
#include "Observer/DisplayElement.h"

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

void headFirstTestMain() {
    //TestStrategy();
    TestObserver();
}
