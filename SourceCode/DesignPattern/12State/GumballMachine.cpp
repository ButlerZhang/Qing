#pragma once
#include "GumballMachine.h"
#include "State.h"

GumballMachine::GumballMachine(int numberGumballs) {
    m_soldOutState = new SoldOutState(this);
    m_noQuarterState = new NoQuarterState(this);
    m_hasQuarterState = new HasQuarterState(this);
    m_soldState = new SoldState(this);
    m_winnerState = new WinnerState(this);
    m_Count = numberGumballs;
    m_State = (numberGumballs > 0) ? m_noQuarterState : m_soldOutState;
}

void GumballMachine::insertQuarter() {
    m_State->insertQuarter();
}

void GumballMachine::ejectQuarter() {
    m_State->ejectQuarter();
}

void GumballMachine::turnCrank() {
    m_State->turnCrank();
    m_State->dispense();
}

void GumballMachine::setState(State *state) {
    m_State = state;
}

void GumballMachine::releaseBall() {
    std::cout << "A gumball comes rolling out the slot..." << std::endl;
    if (m_Count != 0) {
        m_Count -= 1;
    }
}

void GumballMachine::refill(int numberGumballs) {
    m_Count = numberGumballs;
    setState(m_noQuarterState);
}

void TestState() {
    GumballMachine gumballMachine(5);
    gumballMachine.insertQuarter();
    gumballMachine.turnCrank();

    gumballMachine.insertQuarter();
    gumballMachine.turnCrank();
    gumballMachine.insertQuarter();
    gumballMachine.turnCrank();
}
