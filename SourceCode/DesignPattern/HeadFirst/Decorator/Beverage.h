#pragma once
#include <string>

enum CupSize {
    CP_TALL,
    CP_GRANDE,
    CP_VENTI,
    CP_COUNT
};

class Beverage {
public:

    Beverage() : m_cupSize(CP_TALL) {}
    virtual ~Beverage() {}
    virtual double cost() = 0;

    void setSize(CupSize size) {
        m_cupSize = size;
    }

    int getSize() {
        return m_cupSize;
    }

    virtual std::string getDescription() {
        return m_description;
    }

protected:

    CupSize m_cupSize;
    std::string m_description;
};
