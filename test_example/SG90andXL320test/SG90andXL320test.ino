
#include "BluetoothSerial.h"
#define BUFFER_SIZE 100
BluetoothSerial SerialBT;
uint8_t dataBT[BUFFER_SIZE];

#include "XL320.h"
#define BROADCAST_ID 254
XL320 xl320[2];
HardwareSerial Serial_0(0);//(RX, TX) (3, 1)
HardwareSerial Serial_2(2);//(RX, TX) (16, 17)
bool directionFlag[2] = {false, false};

#include <ESP32Servo.h>
Servo sg90[2];
int sg90Pin[2] = {25, 26};

void init_xl320() {
  Serial_0.begin(115200);
  Serial_2.begin(115200);
  xl320[0].begin(Serial_0);
  xl320[1].begin(Serial_2);
  for(int i = 0; i < 2; i++) {
    xl320[i].TorqueON(BROADCAST_ID);
    xl320[i].LED(BROADCAST_ID, "g" );
    xl320[i].setJointSpeed(BROADCAST_ID, 0);
    delay(500);
  }
  
}

void init_sg90() {
  for(int i = 0; i < 2; i++) {
    sg90[i].setPeriodHertz(50);
    sg90[i].attach(sg90Pin[i], 500, 2400);
    sg90[i].write(90);
    delay(500);
  }
}


void setup() {
  Serial.begin(115200);
  //SerialBT.begin("ESP32");
  init_sg90();
  init_xl320();
}

void loop() {
  
  for(int i = 0; i < 2; i++) {
    if(directionFlag[i]) {
      xl320[i].setJointSpeed(BROADCAST_ID, 1023);
      directionFlag[i] = false;
    }
    else {
      xl320[i].setJointSpeed(BROADCAST_ID, 2047);
      directionFlag[i] = true;
    }
    sg90[i].write(90);
    delay(1000);
    xl320[i].setJointSpeed(BROADCAST_ID, 0);
    sg90[i].write(180);
    delay(1000);
  }

}
