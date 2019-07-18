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
int servoID[2] = {5, 6};
int xl320Position = 0;

void init_xl320() {
  Serial_default.begin(115200);
  xl320.begin(Serial_default);
  for(int i = 0; i < 2; i++) {
    xl320.LED(servoID[i], "g" );
  }
}


void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32");
  init_xl320();
}

void loop() {
  xl320.setJointSpeed(servoID[0],2047);
  xl320.setJointSpeed(servoID[1],1023);
  delay(1000);

}