#ifndef MOTOR_HPP_INCLUDE
#define MOTOR_HPP_INCLUDE

#define XL320_NUM 2
#define SG90_NUM 2
#define BROADCAST_ID 254

#include <HardwareSerial.h>
#include <addFuncXL320.hpp>
#include "ESP32Servo.h"

class Motor {

public:
    //XL320
    HardwareSerial ServoSerial[2] = { HardwareSerial(2), HardwareSerial(0) };
    AddFuncXL320 xl320[2];
    //SG90
    Servo sg90[SG90_NUM];

    Motor();
    void setServoDirection(bool direction);
    void startServo(int num);
    void stopServo(int num);

private:
    //XL320
    const int id[XL320_NUM*2] = {5, 7, 6, 8};//Not used but no problem. If use, use to debug.
    bool servoDirection;
    //SG90
    const int pin[SG90_NUM] = {A19, A18};

    void init_xl320();
    void init_sg90();

};

#endif
