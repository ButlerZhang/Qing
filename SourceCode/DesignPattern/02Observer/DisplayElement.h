#pragma once
#include <iostream>
#include "WeatherData.h"

//显示基类
class DisplayElement {
public:

    virtual ~DisplayElement() {}
    virtual void display() = 0;
};

//目前状况布告板：显示当前观测值
class CurrentConfitionDisplay : public Observer, public DisplayElement {
public:

    CurrentConfitionDisplay(Subject* weatherData) {
        m_humidity = 0;
        m_temperature = 0;
        m_Subject = weatherData;
        m_Subject->registerObserver(this);
    }

    virtual void update(float temperature, float humidity, float pressure) {
        m_temperature = temperature;
        m_humidity = humidity;
        display();
    }

    virtual void display() {
        std::cout << "Current conditions: " << m_temperature
            << "F degress and " << m_humidity << "% humidity" << std::endl;
    }

private:

    float m_temperature;
    float m_humidity;
    Subject* m_Subject;
};

//统计布告板：显示最小、平均和最大的观测值
class StatisticsDisplay : public Observer, public DisplayElement {
public:

    StatisticsDisplay(Subject* weatherData) {
        m_counter = 0;
        m_temperatureMin = FLT_MAX;
        m_temperatureMax = FLT_MIN;
        m_temperatureTotal = 0;
        m_Subject = weatherData;
        m_Subject->registerObserver(this);
    }

    virtual void update(float temperature, float humidity, float pressure) {
        ++m_counter;
        m_temperatureTotal += temperature;

        if (temperature > m_temperatureMax) {
            m_temperatureMax = temperature;
        }

        if (temperature < m_temperatureMin) {
            m_temperatureMin = temperature;
        }

        display();
    }

    virtual void display() {
        std::cout << "Statistics: Avg/Max/Min temperature = "
            << m_temperatureTotal / m_counter << "/"
            << m_temperatureMax << "/"
            << m_temperatureMin << std::endl;
    }

private:

    int m_counter;
    float m_temperatureMax;
    float m_temperatureMin;
    float m_temperatureTotal;
    Subject* m_Subject;
};

//预测布告板：显示天气预报
class ForecastDisplay : public Observer, public DisplayElement {
public:

    ForecastDisplay(Subject* weatherData) {
        m_Subject = weatherData;
        m_Subject->registerObserver(this);
    }

    virtual void update(float temperature, float humidity, float pressure) {
        display();
    }

    virtual void display() {
        std::cout << "Forecast: TODO" << std::endl;
    }

private:

    Subject* m_Subject;
};



void TestObserver() {
    WeatherData weatherData;
    CurrentConfitionDisplay currentDiisplay(&weatherData);
    StatisticsDisplay statisticsDisplay(&weatherData);
    ForecastDisplay forecastDisplay(&weatherData);

    weatherData.setMeasurements(80, 65, 30.4f);
    weatherData.setMeasurements(82, 70, 29.2f);
    weatherData.setMeasurements(78, 90, 29.2f);
}
