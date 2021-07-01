#pragma once
#include "State/GumballMachine.h"




void TestState() {
    GumballMachine gumballMachine(5);
    gumballMachine.insertQuarter();
    gumballMachine.turnCrank();

    gumballMachine.insertQuarter();
    gumballMachine.turnCrank();
    gumballMachine.insertQuarter();
    gumballMachine.turnCrank();
}
