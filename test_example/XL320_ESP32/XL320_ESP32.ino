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
int servoID = 1;
int xl320Position = 0;

void init_xl320() {
  Serial_default.begin(115200);
  xl320.begin(Serial_default);
  xl320.setJointSpeed(servoID, 1023);
  xl320.LED(servoID, "g" );
}

void inputPosition(int value) {
  if(value >= 0 && value <= 1023) {
    xl320Position = value;
  }
  else {
    xl320Position = 0;
  }
}

void writeStringToDataBT(uint8_t buffer[], String str) {
  char c_buf[BUFFER_SIZE];
  int data_length = str.length();
  if(data_length < BUFFER_SIZE -1) {
    str.toCharArray(c_buf, data_length);//String to char[]
    for(int i = 0; i < data_length; i++) {//char[] to uint8_t[]
      buffer[i] = (uint8_t)c_buf[i];
    }
    buffer[data_length-1] = '\n';
    buffer[data_length] = '\0';
    SerialBT.write(dataBT, data_length);//write BluetoothSerial
  }
}

bool readSerialBT() {
  String str_buf;
  while(1) {
    if(SerialBT.available() > 0) {
      str_buf = SerialBT.readStringUntil('\n');//read BluetoothSerial buffer
      inputPosition(str_buf.toInt());
      writeStringToDataBT(dataBT, str_buf);
      break;
    }
    else {
      return false;
    }
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32");
  init_xl320();
}

void loop() {

  //xl320
  if(readSerialBT()) {
    xl320.moveJoint(servoID, xl320Position);
  }
  delay(100);

}
