#pragma once

class State;

//糖果机
class GumballMachine {
public:

    GumballMachine(int numberGumballs);

    void insertQuarter();
    void ejectQuarter();
    void turnCrank();

    void setState(State *state);
    void releaseBall();
    void refill(int numberGumballs);

    inline int getCount()const { return m_Count; }
    inline State *getSoldState() { return m_soldState; }
    inline State *getSoldOutState() { return m_soldOutState; }
    inline State *getNoQuarterState() { return m_noQuarterState; }
    inline State *getHasQuarterState() { return m_hasQuarterState; }
    inline State *getWinnerState() { return m_winnerState; }

private:

    State *m_soldOutState;
    State *m_noQuarterState;
    State *m_hasQuarterState;
    State *m_soldState;
    State *m_winnerState;

    int m_Count;                //糖果数目
    State *m_State;             //当前状态
};



void TestState();
