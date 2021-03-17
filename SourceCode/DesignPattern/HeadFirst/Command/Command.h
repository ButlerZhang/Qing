#pragma once
#include "Tools.h"

class Command {
public:

    virtual void execute() = 0;
    virtual void undo() = 0;
};

class NoCommand : public Command {
public:

    virtual void execute() {
        std::cout << "no command" << std::endl;
    }

    virtual void undo() {
        std::cout << "undo" << std::endl;
    }
};

class LightOnCommand : public Command {
public:

    LightOnCommand(Light* light) {
        m_light = light;
    }

    virtual void execute() {
        m_light->on();
    }

    virtual void undo() {
        m_light->off();
    }

private:

    Light* m_light;
};

class LightOffCommand : public Command {
public:

    LightOffCommand(Light* light) {
        m_light = light;
    }

    virtual void execute() {
        m_light->off();
    }

    virtual void undo() {
        m_light->on();
    }

private:

    Light* m_light;
};

class GargeDoorOpenCommand : public Command {
public:

    GargeDoorOpenCommand(GarageDoor* gargeDoor) {
        m_garageDoor = gargeDoor;
    }

    virtual void execute() {
        m_garageDoor->up();
    }

    virtual void undo() {
        m_garageDoor->down();
    }

private:

    GarageDoor* m_garageDoor;
};

class GargeDoorCloseCommand : public Command {
public:

    GargeDoorCloseCommand(GarageDoor* gargeDoor) {
        m_garageDoor = gargeDoor;
    }

    virtual void execute() {
        m_garageDoor->down();
    }

    virtual void undo() {
        m_garageDoor->up();
    }

private:

    GarageDoor* m_garageDoor;
};
