#pragma once
#include "Beverage.h"

class HouseBlend : public Beverage {
public:

    HouseBlend() {
        m_description = "House Blend Coffee";
    }

    virtual double cost() {
        return 0.89;
    }
};

class DarkRoast : public Beverage {
public:

    DarkRoast() {
        m_description = "Dark Roast Coffee";
    }

    virtual double cost() {
        return 1.2;
    }
};

class Espresso : public Beverage {
public:

    Espresso() {
        m_description = "Espresso";
    }

    virtual double cost() {
        return 1.99;
    }
};

class Decaf : public Beverage {
public:

    Decaf() {
        m_description = "Decaf Coffee";
    }

    virtual double cost() {
        return 0.88;
    }
};
