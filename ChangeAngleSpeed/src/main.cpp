#include "footSystem.hpp"

//Pin[] = {PhotoSensor, RotaryEncoderA, RotaryEncoderB, SG90};
FootSystem* RightFoot;
const int RightPin[] = {A4, 34, 35, 26};
FootSystem* LeftFoot;
const int LeftPin[] = {A5, 36, 39, 25};

unsigned long startTime = 0;
unsigned long stopTime = 0;
unsigned long runTime = 0;
int runCount = 0;

int angle = 10;
bool dataFlag = false;

void run();
bool setAngle();

void setup() {
    Serial.begin(115200);

    Serial.println(F("RightFoot setup"));
    RightFoot = new FootSystem(RightPin, "right");
    Serial.println(F("LeftFoot setup"));
    LeftFoot = new FootSystem(LeftPin, "left");

    Serial.println(F("waite 5s"));
    delay(5000);
}

void loop() {
    

    if(!dataFlag) {
        delay(100);
        if(runCount == 100) {
        runTime /= runCount;
        Serial.print(F("average runTime : "));
        Serial.println(runTime);
        Serial.println();
        Serial.println(F("finish"));
        while(1);
        }
        runCount++;
        Serial.println(runCount);
        startTime = millis();
        setAngle();
    }
    else {
        run();
    }

}

void run() {
    // RightFoot->run();
    LeftFoot->run();
    if(/*RightFoot->getEndFlag() &&*/
        LeftFoot->getEndFlag()) {
        dataFlag = false;
        // RightFoot->setEndFlag(false);
        LeftFoot->setEndFlag(false);
        stopTime = millis();
        runTime += stopTime - startTime;
    }
}

bool setAngle() {
    int value = runCount % 2 == 0 ? angle : (-1) * angle;
    if(runCount == 100) value = 0;
    dataFlag = /*RightFoot->setAngle(value) && */LeftFoot->setAngle(value) ?
     true : false;
    return dataFlag;
}