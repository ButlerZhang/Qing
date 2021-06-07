#pragma once
#include <iostream>

class QuackBehavior {
public:

    virtual ~QuackBehavior() {
    }

    virtual void quack() = 0;
};

class Quack : public QuackBehavior {
public:

    virtual void quack() {
        std::cout << "Quack" << std::endl;
    }
};

class MuteQuack : public QuackBehavior {
public:

    virtual void quack() {
        std::cout << "<< Silence >>" << std::endl;
    }
};

class Squeak : public QuackBehavior {
public:

    virtual void quack() {
        std::cout << "Squeak" << std::endl;
    }
};
