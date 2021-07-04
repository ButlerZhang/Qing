#pragma once
#include <iostream>
#include "Quackable.h"

//¶ì
class CGoose {
public:
    virtual void honk() {
        std::cout << "Honk" << std::endl; //¶ìµÄ½ÐÉù
    }
};

//ÊÊÅäÆ÷Ä£Ê½£º¶ì½ÐÉùÊÊÅäÆ÷
class CGooseAdapter : public Quackable {
public:

    CGooseAdapter(CGoose *goose) {
        m_goose = goose;
        m_observable = new Observable(this);
    }

    virtual void quack() {
        m_goose->honk();
    }

    virtual std::string getName() {
        return "goose";
    }

    virtual void registerObserver(CObserver *observer) {
        m_observable->registerObserver(observer);
    }

    virtual void notifyObservers() {
        m_observable->notifyObservers();
    }

private:
    Observable *m_observable;
    CGoose *m_goose;
};
