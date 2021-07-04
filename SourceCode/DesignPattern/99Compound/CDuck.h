#pragma once
#include <iostream>
#include "Quackable.h"

//ÂÌÍ·Ñ¼
class CMallardDuck : public Quackable {
public:

    CMallardDuck() {
        m_observable = new Observable(this);
    }

    virtual void quack() {
        std::cout << "Quack" << std::endl;
        notifyObservers();
    }

    virtual std::string getName() {
        return "mallard duck";
    }

    virtual void registerObserver(CObserver *observer) {
        m_observable->registerObserver(observer);
    }

    virtual void notifyObservers() {
        m_observable->notifyObservers();
    }

private:
    Observable *m_observable;
};

//ºìÍ·Ñ¼
class CRedHeadDuck : public Quackable {
public:

    CRedHeadDuck() {
        m_observable = new Observable(this);
    }

    virtual void quack() {
        std::cout << "Quack" << std::endl;
        notifyObservers();
    }

    virtual std::string getName() {
        return "red head duck";
    }

    virtual void registerObserver(CObserver *observer) {
        m_observable->registerObserver(observer);
    }

    virtual void notifyObservers() {
        m_observable->notifyObservers();
    }

private:
    Observable *m_observable;
};

//Ñ¼ÃùÆ÷
class CDuckCall : public Quackable {
public:

    CDuckCall() {
        m_observable = new Observable(this);
    }

    virtual void quack() {
        std::cout << "Kwak" << std::endl;
        notifyObservers();
    }

    virtual std::string getName() {
        return "duck call";
    }

    virtual void registerObserver(CObserver *observer) {
        m_observable->registerObserver(observer);
    }

    virtual void notifyObservers() {
        m_observable->notifyObservers();
    }

private:
    Observable *m_observable;
};

//ÏðÆ¤Ñ¼
class CRubberDuck : public Quackable {
public:

    CRubberDuck() {
        m_observable = new Observable(this);
    }

    virtual void quack() {
        std::cout << "Squeak" << std::endl;
        notifyObservers();
    }

    virtual std::string getName() {
        return "rubber duck";
    }

    virtual void registerObserver(CObserver *observer) {
        m_observable->registerObserver(observer);
    }

    virtual void notifyObservers() {
        m_observable->notifyObservers();
    }

private:
    Observable *m_observable;
};
