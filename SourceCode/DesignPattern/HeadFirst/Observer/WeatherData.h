#pragma once
#include <list>
#include "Subject.h"
#include "Observer.h"

class WeatherData : public Subject {
public:

    WeatherData() {
        m_temperature = 0;
        m_humidity = 0;
        m_pressure = 0;
    }

    virtual void registerObserver(Observer* pO) {
        m_observers.push_back(pO);
    }

    virtual void removeObserver(Observer* pO) {
        std::list<Observer*>::iterator it;
        it = std::find(m_observers.begin(), m_observers.end(), pO);
        if (it != m_observers.end()) {
            m_observers.erase(it);
        }
    }

    virtual void notifyObserver() {
        for (auto it = m_observers.cbegin(); it != m_observers.cend(); it++) {
            (*it)->update(m_temperature, m_humidity, m_pressure);
        }
    }

    float getTemperature() {
        return 0;
    }

    float getHumidity() {
        return 0;
    }

    float getPressure() {
        return 0;
    }

    void measurementsChanged() {
        notifyObserver();
    }

    void setMeasurements(float temperature, float humidity, float pressure) {
        m_temperature = temperature;
        m_humidity = humidity;
        m_pressure = pressure;
        measurementsChanged();
    }

private:

    std::list<Observer*> m_observers;
    float m_temperature;
    float m_humidity;
    float m_pressure;
};
