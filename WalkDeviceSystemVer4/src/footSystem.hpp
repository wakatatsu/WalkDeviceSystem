#ifndef FOOTSYSTEM_HPP_INCLUDE
#define FOOTSYSTEM_HPP_INCLUDE

#include "sensor.hpp"
#include "motor.hpp"

class FootSystem {

public:
    FootSystem(const int pins[]);
    FootSystem(const int pin[], int footType);
    void runDebug();
    void run();
    bool setAngle(int angle);
    void setEndFlag(bool flag);
    bool getEndFlag();

private:
    //Sensor and Motor Object
    Motor* motor;
    Sensor* sensor;
    bool moveFlag = false;
    bool endFlag = false;

};

#endif