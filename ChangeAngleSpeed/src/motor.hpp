#ifndef MOTOR_HPP_INCLUDE
#define MOTOR_HPP_INCLUDE

#define BROADCAST_ID 254

#include <HardwareSerial.h>
#include <addFuncXL320.hpp>
#include "ESP32Servo.h"

class Motor {

public:
  //XL320
  HardwareSerial* ServoSerial;
  AddFuncXL320 xl320;
  //SG90
  Servo sg90;

  Motor(const int pin[], const char* footType);
  void setServoDirection(bool direction);
  void startServo();
  void stopServo();

private:
  //foot type
  const char* type;
  //XL320
  const int id[4] = {5, 7, 6, 8};//Not used but no problem. If use, use to debug.
  bool servoDirection;
  //SG90
  const int sg90Pin;

  void init_xl320();
  void init_sg90();

};

#endif
