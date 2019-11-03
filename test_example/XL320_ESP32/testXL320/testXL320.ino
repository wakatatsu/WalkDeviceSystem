/*
input position value is sent value by bluetoothSerial.
0 <= position value <= 1023
*/
#include "BluetoothSerial.h"
#define BUFFER_SIZE 100
BluetoothSerial SerialBT;
uint8_t dataBT[BUFFER_SIZE];

#include "XL320.h"
XL320 xl320;
HardwareSerial Serial_default(0);//(RX, TX) (3, 1)
int servoID[4] = {5, 6, 7, 8};
int xl320Position = 0;

#include <ESP32Servo.h>
Servo sg90[2];
int sg90Pin[2] = {A19, A18};
int sg90Position = 0;
int value = 0;

void init_xl320() {
  Serial_default.begin(115200);
  xl320.begin(Serial_default);
  for(int i = 0; i < 4; i++) {
    xl320.TorqueON(servoID[i]);
    xl320.LED(servoID[i], "g" );
    xl320.setJointSpeed(servoID[i], 0);
  }
}

void init_sg90() {
  for(int i = 0; i < 2; i++) {
    sg90[i].setPeriodHertz(50);
    sg90[i].attach(sg90Pin[i], 500, 2400);
    sg90[i].write(90);
  }
  delay(1000);
}


void setup() {
  Serial.begin(115200);
  //SerialBT.begin("ESP32");
  init_xl320();
  init_sg90();
}

void loop() {
//  for(int i = 0; i < 4; i++) {
//    xl320.setJointSpeed(servoID[i],1023);
//    delay(10);
//  }
  xl320.setJointSpeed(254,1023);
  delay(3000);
  xl320.setJointSpeed(254,0);
  delay(3000);

//  for(int i = 2; i < 4; i++) {
//    xl320.setJointSpeed(servoID[i],1023);
//    delay(1);
//  }
//  delay(100);
//  xl320.setJointSpeed(254,0);
//  delay(1000);
  
//  for(int i = 0; i < 2; i++) {
//    xl320.setJointSpeed(servoID[i],2047);
//    delay(1);
//  }
//  delay(100);
//  xl320.setJointSpeed(254,0);
//  delay(1000);

//  for(int i = 2; i < 4; i++) {
//    xl320.setJointSpeed(servoID[i],2047);
//    delay(1);
//  }
//  delay(100);
//  xl320.setJointSpeed(254,0);
//  delay(1000);

}
