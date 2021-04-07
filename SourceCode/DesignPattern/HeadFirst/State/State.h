#pragma once
#include <iostream>
#include "GumballMachine.h"

class State {
public:

    virtual void insertQuarter() = 0;       //投币
    virtual void ejectQuarter() = 0;        //退币
    virtual void turnCrank() = 0;           //转动曲柄
    virtual void dispense() = 0;            //发放糖果

protected:

    GumballMachine *m_machine;
};

class SoldState : public State {
public:

    SoldState(GumballMachine *machine) {
        m_machine = machine;
    }

    virtual void insertQuarter() {
        std::cout << "Please wait, we're already giving you a gumball" << std::endl;
    }

    virtual void ejectQuarter() {
        std::cout << "Sorry, you already turned the crank" << std::endl;
    }

    virtual void turnCrank() {
        std::cout << "Turning twice doesn't get you another gumball!" << std::endl;
    }

    virtual void dispense() {
        m_machine->releaseBall();
        if (m_machine->getCount() > 0) {
            m_machine->setState(m_machine->getNoQuarterState());
        }
        else {
            std::cout << "Oops, out of gumballs!" << std::endl;
            m_machine->setState(m_machine->getSoldOutState());
        }
    }
};

class SoldOutState : public State {
public:

    SoldOutState(GumballMachine *machine) {
        m_machine = machine;
    }

    virtual void insertQuarter() {
        std::cout << "You can't insert a quarter, the machine is sold out" << std::endl;
    }

    virtual void ejectQuarter() {
        std::cout << "You can't eject, you haven't inserted a quarter yet" << std::endl;
    }

    virtual void turnCrank() {
        std::cout << "You turned, but there are no gumballs" << std::endl;
    }

    virtual void dispense() {
        std::cout << "No gumball dispensed" << std::endl;
    }
};

class NoQuarterState : public State {
public:

    NoQuarterState(GumballMachine *machine) {
        m_machine = machine;
    }

    virtual void insertQuarter() {
        std::cout << "You inserted a quarter" << std::endl;
        m_machine->setState(m_machine->getHasQuarterState());
    }

    virtual void ejectQuarter() {
        std::cout << "You haven't inserted a quarter" << std::endl;
    }

    virtual void turnCrank() {
        std::cout << "You turned, but there's no quarter" << std::endl;
    }

    virtual void dispense() {
        std::cout << "You need to pay first" << std::endl;
    }
};

class HasQuarterState : public State {
public:

    HasQuarterState(GumballMachine *machine) {
        m_machine = machine;
    }

    virtual void insertQuarter() {
        std::cout << "You can't insert another quarter." << std::endl;
    }

    virtual void ejectQuarter() {
        std::cout << "Quarter returned" << std::endl;
        m_machine->setState(m_machine->getNoQuarterState());
    }

    virtual void turnCrank() {
        std::cout << "You turned..." << std::endl;
        bool isWinner = (rand() % 10) % 2 == 0;
        if (isWinner && m_machine->getCount() > 1) {
            m_machine->setState(m_machine->getWinnerState());
        }
        else {
            m_machine->setState(m_machine->getSoldState());
        }
    }

    virtual void dispense() {
        std::cout << "No gumball dispensed" << std::endl;
    }
};

class WinnerState : public State {
public:

    WinnerState(GumballMachine *machine) {
        m_machine = machine;
    }

    virtual void insertQuarter() {
        std::cout << "Please wait, we're already giving you a gumball" << std::endl;
    }

    virtual void ejectQuarter() {
        std::cout << "Sorry, you already turned the crank" << std::endl;
    }

    virtual void turnCrank() {
        std::cout << "Turning twice doesn't get you another gumball!" << std::endl;
    }

    virtual void dispense() {
        std::cout << "YOU'RE A WINNER! You get two gumballs for your quarter" << std::endl;
        m_machine->releaseBall();
        if (m_machine->getCount() == 0) {
            m_machine->setState(m_machine->getSoldOutState());
        }
        else {
            m_machine->releaseBall();
            if (m_machine->getCount() > 0) {
                m_machine->setState(m_machine->getNoQuarterState());
            }
            else {
                std::cout << "Oops, out of gumballs!" << std::endl;
                m_machine->setState(m_machine->getSoldOutState());
            }
        }
    }
};
