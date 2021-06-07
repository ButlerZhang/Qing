#pragma once
#include <iostream>

class FlyBehavior {
public:

    virtual ~FlyBehavior() {
    }

    virtual void fly() = 0;
};

class FlyWithWings : public FlyBehavior {
public:

    virtual void fly() {
        std::cout << "I'm flying!" << std::endl;
    }
};

class FlyNoWay : public FlyBehavior {
public:

    virtual void fly() {
        std::cout << "I can't fly." << std::endl;
    }
};

class FlyRocketPowered : public FlyBehavior {
public:

    virtual void fly() {
        std::cout << "I'm flying with a rocket!" << std::endl;
    }
};
