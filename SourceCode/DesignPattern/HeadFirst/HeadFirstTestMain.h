#pragma once
#include "Command/Control.h"
#include "Command/Tools.h"
#include "Iterator/PancakeHouseMenu.h"
#include "Iterator/DinerMenu.h"
#include "Iterator/Waitress.h"
#include "State/GumballMachine.h"





void TestCommand() {
    SimpleRemoteControl *remote = new SimpleRemoteControl();
    Light *light = new Light();
    GarageDoor *garageDoor = new GarageDoor();

    LightOnCommand *lightOn = new LightOnCommand(light);
    GargeDoorOpenCommand *garageOpen = new GargeDoorOpenCommand(garageDoor);

    remote->setCommand(lightOn);
    remote->buttonWasPressed();

    remote->setCommand(garageOpen);
    remote->buttonWasPressed();
}

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
