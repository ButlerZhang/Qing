#pragma once
#include <iostream>

class Light {
public:
    void on() {
        std::cout << "Light is on" << std::endl;
    }

    void off() {
        std::cout << "Light is off" << std::endl;
    }
};

class GarageDoor {
public:

    void up() {
        std::cout << "Garage Door is up" << std::endl;
    }

    void down() {
        std::cout << "Garage Door is down" << std::endl;
    }

    void stop() {
        std::cout << "Garage Door is stop" << std::endl;
    }

    void lightOn() {
        std::cout << "Garage Door is light on" << std::endl;
    }

    void lightOff() {
        std::cout << "Garage Door is light off" << std::endl;
    }
};
