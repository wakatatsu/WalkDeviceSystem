#include "BluetoothSerial.h"
#include "sensor.hpp"
#include "motor.hpp"

#define BUFFER_SIZE 100

//Bluetooth Serial
BluetoothSerial SerialBT;
uint8_t dataBT[BUFFER_SIZE];//bluetooth data buffer
bool dataBTFlag = false;

Sensor* sensor;
Motor* motor;

// //photo sensor
const int photoThreshold[2] = {100, 100};
bool currentPhotoFlag[2];
bool beforPhotoFlag[2];
int walkCount;

//angle
int currentAngle[2];
int goalAngle[2];
bool endFlag[2] = {false, false};

bool readSerialBT();
void writeStringToDataBT(uint8_t buffer[], String str);

void setGoalAngle(int value);
void setDirection(int value);
bool moveStartSwitch(int arrayNum);


void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32");
  sensor = new Sensor();
  motor = new Motor();
}
//
void loop() {

  //read angle data
  if(readSerialBT()) {
    dataBTFlag = true;
  }

  //run sysytem
  while(dataBTFlag) {
    for(int i = 0; i < 2; i++) {
      if(moveStartSwitch(i)) {
        motor->startServo(i);
        endFlag[i] = false;
      }
      if (goalAngle[i]-2 >= sensor->getDegValue(i) &&
          goalAngle[i]+2 <= sensor->getDegValue(i)) {
        motor->stopServo(i);
        endFlag[i] = true;
      }
    }
    if(endFlag[0] && endFlag[1]) {
      dataBTFlag = false;
      break;
    }
  }
}

//-----------------------------------------------
//bluetoothSerial func

bool readSerialBT() {
  String str_buf;
  while(1) {
    if(SerialBT.available() > 0) {
      str_buf = SerialBT.readStringUntil('\n');//read BluetoothSerial buffer
      setGoalAngle(str_buf.toInt());
      //writeStringToDataBT(dataBT, str_buf);
      break;
    }
    else {
      return false;
    }
  }
  return true;
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
    SerialBT.write(buffer, data_length);//write BluetoothSerial
  }
}

// //-----------------------------------------------
void setGoalAngle(int value) {
  goalAngle[0] = currentAngle[0] + value;
  goalAngle[1] = currentAngle[1] + value;
  setDirection(value);
}
void setDirection(int value) {
  if(value > 0) {//right
    motor->setServoDirection(false);
  }
  else if(value < 0) {//left
    motor->setServoDirection(true);
  }
}

bool moveStartSwitch(int arrayNum) {
  int readValue = sensor->readPhotoValue(arrayNum);
  beforPhotoFlag[arrayNum] = currentPhotoFlag[arrayNum];
  currentPhotoFlag[arrayNum] = (readValue > photoThreshold[arrayNum]) ? true : false;
  if(beforPhotoFlag[arrayNum] && !currentPhotoFlag[arrayNum]) {
    walkCount++;
    if(walkCount==10) walkCount = 0;
    return true;
  }
  else {
    return false;
  }
}
