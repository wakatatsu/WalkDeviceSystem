#include "motor.hpp"

Motor::Motor(const int pins[], const char* footType) : type(footType), sg90Pin(pins[3]) {
  ServoSerial = !strcmp(type, "right") ? new HardwareSerial(2) : new HardwareSerial(0);
  init_xl320();
  init_sg90();
}

void Motor::init_xl320() {
  ServoSerial->begin(115200);
  xl320.begin(*ServoSerial);
  xl320.TorqueON(BROADCAST_ID);
  xl320.LED(BROADCAST_ID, "g");//Warning this method but no problem.
  xl320.moveWheel(BROADCAST_ID, 0);
}

void Motor::init_sg90() {
  sg90.setPeriodHertz(50);
  sg90.attach(sg90Pin, 500, 2400);
  sg90.write(180);
}

void Motor::setServoDirection(bool direction) {
  servoDirection = direction;
}

void Motor::startServo() {
  sg90.write(90);
  delay(1);
  if(servoDirection) {
    xl320.moveWheel(BROADCAST_ID, 2047);
  }
  else {
    xl320.moveWheel(BROADCAST_ID, 1023);
  }
}

void Motor::stopServo() {
  xl320.moveWheel(BROADCAST_ID, 0);
  sg90.write(180);
}
