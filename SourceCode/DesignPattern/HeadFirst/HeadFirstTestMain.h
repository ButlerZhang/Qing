#pragma once
#include "Iterator/PancakeHouseMenu.h"
#include "Iterator/DinerMenu.h"
#include "Iterator/Waitress.h"
#include "State/GumballMachine.h"







void TestIterator() {
    Waitress waitress;
    waitress.printMenu();
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

void headFirstTestMain() {
    //TestObserver();
    //TestDecorator();
    //TestFactoryMethod();
    //TestCommand();
    //TestIterator();
    //TestState();
}
