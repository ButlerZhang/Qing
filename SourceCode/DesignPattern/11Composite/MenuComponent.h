#pragma once
#include <string>
#include <exception>

//��֧�ֵĲ����׳����쳣
class UnSupportedOperationException : public std::exception {
public:
    UnSupportedOperationException() : std::exception("un-supported operation exception") {
    }
};

//�˵��������
class MenuComponent {
public:

    MenuComponent() {
        m_guid = 0; //getGUID();
    }

    int getGUID() {
        return m_guid;
    }

    virtual void add(MenuComponent *menuComponent) {
        throw UnSupportedOperationException();
    }

    virtual void remove(MenuComponent *menuComponent) {
        throw UnSupportedOperationException();
    }

    virtual MenuComponent* getChild(int i) {
        throw UnSupportedOperationException();
    }

    virtual std::string getName() {
        throw UnSupportedOperationException();
    }

    virtual std::string getDescription() {
        throw UnSupportedOperationException();
    }

    virtual double getPrice() {
        throw UnSupportedOperationException();
    }

    virtual bool isVegetarian() {
        throw UnSupportedOperationException();
    }

    virtual void print() {
        throw UnSupportedOperationException();
    }

private:

    int m_guid;
};
