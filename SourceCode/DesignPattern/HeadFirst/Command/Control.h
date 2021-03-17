#pragma once
#include "Command.h"

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

