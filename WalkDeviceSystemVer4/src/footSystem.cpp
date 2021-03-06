#include "footSystem.hpp"

FootSystem::FootSystem(const int pins[]) {

}

FootSystem::FootSystem(const int pins[], int footType) {
    sensor = new Sensor(pins, footType);
    motor = new Motor(pins, footType);
}

void FootSystem::runDebug() {
    float photoValue;
    float degValue;
    
    Serial.println(F("test motoer"));
    delay(1000);
    motor->startServo();
    delay(5000);
    motor->stopServo();
    
    // Serial.println(F("test sensor"));
    // delay(1000);
    // for(int i = 0; i < 10000; i++) {
    //     photoValue = sensor->readPhotoValue();
    //     Serial.print(F("PhotoValue:"));
    //     Serial.print(photoValue);
    //     degValue = sensor->getDegValue();
    //     // degValue = sensor->getCountValue();
    //     Serial.print(F(", AngleValue:"));
    //     Serial.println(degValue);
    //     delay(1);
    // }
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