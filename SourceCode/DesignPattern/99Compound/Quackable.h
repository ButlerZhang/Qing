#pragma once
#include <vector>
#include "QuackObservable.h"

//叫声接口
class Quackable : public QuackObservable {
public:
    virtual void quack() = 0;
};

//装饰者模式：统计鸭子叫声
class QuackCounter : public Quackable {
public:

    QuackCounter(Quackable *duck) {
        m_duck = duck;
    }

    virtual void quack() {
        m_duck->quack();
        ++m_numberOfQuacks;
    }

    virtual void registerObserver(CObserver *observer) {
        m_duck->registerObserver(observer);
    }

    virtual void notifyObservers() {
        m_duck->notifyObservers();
    }

    static int getNumberOfQuacks() {
        return m_numberOfQuacks;
    }

private:

    Quackable *m_duck;
    static int m_numberOfQuacks;
};

int QuackCounter::m_numberOfQuacks = 0;

//组合模式：一群鸭子
class Flock : public Quackable {
public:

    void add(Quackable *quacker) {
        m_quackers.push_back(quacker);
    }

    virtual void quack() {
        //迭代器模式
        for (auto it = m_quackers.begin(); it != m_quackers.end(); it++) {
            (*it)->quack();
        }
    }

    virtual void registerObserver(CObserver *observer) {
        for (auto it = m_quackers.begin(); it != m_quackers.end(); it++) {
            (*it)->registerObserver(observer);
        }
    }

    virtual void notifyObservers() {
        //由个体去操心，群不需要关注
    }

private:

    std::vector<Quackable *> m_quackers;
};
