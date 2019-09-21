#ifndef MOTOR_HPP_INCLUDE
#define MOTOR_HPP_INCLUDE

#define XL320_NUM 4
#define SG90_NUM 4
#define BROADCAST_ID 254

#include <HardwareSerial.h>
#include "addFuncXL320.hpp"
#include "ESP32Servo.h"

class Motor {

public:
  //XL320
  HardwareSerial Serial_default = HardwareSerial(0);
  AddFuncXL320 xl320;
  //SG90
  Servo sg90[SG90_NUM];

  Motor();
  void setServoDirection(bool direction);
  void startServo(int num);
  void stopServo(int num);

private:
  //XL320
  const int id[XL320_NUM] = {5, 6, 7, 8};
  bool servoDirection;
  //SG90
  const int pin[SG90_NUM] = {A4, A5, A18, A19};

  void init_xl320();
  void init_sg90();

};

#endif
