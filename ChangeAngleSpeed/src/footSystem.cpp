#include "footSystem.hpp"

FootSystem::FootSystem(const int pins[], const char* footType) {
    sensor = new Sensor(pins, footType);
    motor = new Motor(pins, footType);
}

void FootSystem::runDebug() {
    float value;
    motor->startServo();
    for(int i = 0; i < 10000; i++) {
        value = sensor->readPhotoValue();
        Serial.print(F("PhotoValue:"));
        Serial.print(value);
        value = sensor->getDegValue();
        Serial.print(F(", AngleValue"));
        Serial.println(value);
        delay(1);
    }
    motor->stopServo();
}

void FootSystem::run() {
    if(!moveFlag && !endFlag) {
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

bool FootSystem::setAngle(int angle) {
    if(angle == sensor->getGoalAngle()) return false;
    //setGoalAngle() return bool of direction.
    motor->setServoDirection(sensor->setGoalAngle(angle));
    return true;
}

void FootSystem::setEndFlag(bool flag) {
    endFlag = flag;
}

bool FootSystem::getEndFlag() {
    return endFlag;
}