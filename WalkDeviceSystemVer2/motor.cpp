#include "motor.hpp"

Motor::Motor() {
  init_xl320();
  init_sg90();
}

void Motor::init_xl320() {
  Serial_default.begin(115200);
  xl320.begin(Serial_default);
  for(int i = 0; i < XL320_NUM; i++) {
    xl320.TorqueON(id[i]);
    xl320.LED(id[i], "r");
    xl320.moveWheel(id[i], 0);
  }
}

void Motor::init_sg90() {
  for(int i = 0; i < SG90_NUM; i++) {
    sg90[i].setPeriodHertz(50);
    sg90[i].attach(pin[i], 500, 2400);
    sg90[i].write(0);
  }
}

void Motor::setServoDirection(bool direction) {
  servoDirection = direction;
}

void Motor::startServo(int num) {
  sg90[num].write(90);
  sg90[num+2].write(90);
  if(servoDirection) {
    xl320.moveWheel(id[num], 1023);
    xl320.moveWheel(id[num+2], 1023);
  }
  else {
    xl320.moveWheel(id[num], 2047);
    xl320.moveWheel(id[num+2], 2047);
  }
}

void Motor::stopServo(int num) {
  xl320.moveWheel(BROADCAST_ID, 0);
  sg90[num].write(0);
  sg90[num+2].write(0);
}
