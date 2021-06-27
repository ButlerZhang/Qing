#pragma once
#include "Command.h"
#include <vector>

class SimpleRemoteControl {
public:

    SimpleRemoteControl() {
        m_slot = nullptr;
    }

    void setCommand(Command* command) {
        m_slot = command;
    }

    void buttonWasPressed() {
        m_slot->execute();
    }

private:

    Command* m_slot;
};

class RemoteControl {
public:

    RemoteControl() {
        Command* noCommand = new NoCommand();
        m_undoCommand = noCommand;
        for (int i = 0; i < 7; i++) {
            m_OnCommand.push_back(noCommand);
            m_OffCommand.push_back(noCommand);
        }
    }

    void setCommand(int slot, Command* onCommand, Command* offCommand) {
        m_OnCommand[slot] = onCommand;
        m_OffCommand[slot] = offCommand;
    }

    void onButtonWasPushed(int slot) {
        m_OnCommand[slot]->execute();
        m_undoCommand = m_OnCommand[slot];
    }

    void offButtonWasPushed(int slot) {
        m_OffCommand[slot]->execute();
        m_undoCommand = m_OffCommand[slot];
    }

    void undoButtonWasPushed() {
        m_undoCommand->undo();
    }

private:

    Command* m_undoCommand;
    std::vector<Command*> m_OnCommand;
    std::vector<Command*> m_OffCommand;
};



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

    //因为SimpleRemoteControl已经足够说明命令模式了，
    //所以这里省略了RemoteControl的测试代码。
}