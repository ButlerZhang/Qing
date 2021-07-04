#pragma once
#include <vector>
#include "QuackObservable.h"

//�����ӿ�
class Quackable : public QuackObservable {
public:
    virtual void quack() = 0;
};

//װ����ģʽ��ͳ��Ѽ�ӽ���
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

//���ģʽ��һȺѼ��
class Flock : public Quackable {
public:

    void add(Quackable *quacker) {
        m_quackers.push_back(quacker);
    }

    virtual void quack() {
        //������ģʽ
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
        //�ɸ���ȥ���ģ�Ⱥ����Ҫ��ע
    }

private:

    std::vector<Quackable *> m_quackers;
};
