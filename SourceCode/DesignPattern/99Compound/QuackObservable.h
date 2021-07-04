#pragma once
#include <vector>

class QuackObservable;

//�۲��߻���
class CObserver {
public:
    virtual void update(QuackObservable *duck) = 0;
};

//���۲��߻��࣬������
class QuackObservable {
public:
    virtual void registerObserver(CObserver *observer) = 0;
    virtual void notifyObservers() = 0;

    virtual std::string getName() {
        return "nobody";
    }
};

//���۲���������
class Observable : public QuackObservable {
public:

    Observable(QuackObservable *duck) {
        m_duck = duck;
    }

    virtual void registerObserver(CObserver *observer) {
        m_observers.push_back(observer);
    }

    virtual void notifyObservers() {
        for (auto it = m_observers.begin(); it != m_observers.end(); it++) {
            (*it)->update(m_duck);
        }
    }

private:

    QuackObservable *m_duck;
    std::vector<CObserver *> m_observers;
};

//�۲��������ࣺ�۲����ɽ�ѧ��
class Quackologist : public CObserver {
public:
    virtual void update(QuackObservable *duck) {
        std::cout << "Quackologist: " << duck->getName() << " just quacked." << std::endl;
    }
};
