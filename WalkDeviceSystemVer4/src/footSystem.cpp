#include "footSystem.hpp"

FootSystem::FootSystem(const int pins[], const char* footType) {
    sensor = new Sensor(pins, footType);
    motor = new Motor(pins, footType);
}

void FootSystem::run() {
    if(!moveFlag) {
        if(sensor->swingJudge()) {
            Serial.println(F("start"));
            motor->startServo();
            moveFlag = true;
        }
    }
    else {
        if(sensor->moveStopJudge()) {
            Serial.println(F("stop"));
            motor->stopServo();
            moveFlag = false;
            endFlag = true;
        }
    }
}

 void FootSystem::setAngle(int angle) {
     //setGoalAngle return bool of direction.
     motor->setServoDirection(sensor->setGoalAngle(angle));
 }

void FootSystem::setEndFlag(bool flag) {
    endFlag = flag;
}

bool FootSystem::getEndFlag() {
    return endFlag;
}