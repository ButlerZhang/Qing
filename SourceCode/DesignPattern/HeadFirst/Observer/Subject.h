#pragma once
#include <memory>

class Observer;

class Subject {
public:

    virtual ~Subject() {}
    virtual void registerObserver(Observer* pO) = 0;
    virtual void removeObserver(Observer* pO) = 0;
    virtual void notifyObserver() = 0;
};
