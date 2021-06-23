#pragma once

class Observer {
public:

    virtual ~Observer() {}
    virtual void update(float temperature, float humidity, float pressure) = 0;
};
