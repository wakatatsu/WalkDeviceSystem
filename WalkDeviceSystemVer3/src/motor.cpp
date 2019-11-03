#include "motor.hpp"

Motor::Motor() {
  init_xl320();
  init_sg90();
}

void Motor::init_xl320() {
  for(int i = 0; i < XL320_NUM; i++) {
    ServoSerial[i].begin(115200);
    xl320[i].begin(ServoSerial[i]);
    xl320[i].TorqueON(BROADCAST_ID);
    xl320[i].LED(BROADCAST_ID, "g");//Warning this method but no problem.
    xl320[i].moveWheel(BROADCAST_ID, 0);
  }
}

void Motor::init_sg90() {
  for(int i = 0; i < SG90_NUM; i++) {
    sg90[i].setPeriodHertz(50);
    sg90[i].attach(pin[i], 500, 2400);
    sg90[i].write(180);
  }
}

void Motor::setServoDirection(bool direction) {
  servoDirection = direction;
}

void Motor::startServo(int num) {
  sg90[num].write(90);
  if(servoDirection) {
    xl320[num].moveWheel(BROADCAST_ID, 1023);
  }
  else {
    xl320[num].moveWheel(BROADCAST_ID, 2047);
  }
}

void Motor::stopServo(int num) {
  xl320[num].moveWheel(BROADCAST_ID, 0);
  sg90[num].write(180);
}
