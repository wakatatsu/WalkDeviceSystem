#include "BluetoothSerial.h"
#include "sensor.hpp"
#include "motor.hpp"

#define BUFFER_SIZE 100

//Bluetooth Serial
BluetoothSerial SerialBT;
uint8_t dataBT[BUFFER_SIZE];//bluetooth data buffer
bool dataBTFlag = false;

//Sensor and Motor Object
Sensor* sensor;
Motor* motor;

// //photo sensor
const int photoThreshold[2] = {10, 10};
bool currentPhotoFlag[2] = {false, false};
bool beforPhotoFlag[2] = {false, false};
int walkCount;

//angle
int currentAngle[2] = {0, 0};
int goalAngle[2] = {0, 0};
bool endFlag[2] = {false, false};
bool moveFlag[2] = {false, false};

void init_value();

bool readSerialBT();
void writeStringToDataBT(uint8_t buffer[], String str);

void setGoalAngle(int value);
void setDirection(int value);
bool swingJudge(int arrayNum);
bool moveStopJudge(int arrayNum);


void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32");
  sensor = new Sensor();
  motor = new Motor();
}

void loop() {
  if(!dataBTFlag) {
    if(readSerialBT()) {//set goal value if read
      Serial.println("read");
      dataBTFlag = true;
    }
  }
  else {
    for(int i = 0; i < 2; i++) {
      if(!moveFlag[i]) {
        if(swingJudge(i)) {
          Serial.println("start");
          motor->startServo(i);
          moveFlag[i] = true;
        }
      }
      else {
        if(moveStopJudge(i)) {
          Serial.println("stop");
          motor->stopServo(i);
          moveFlag[i] = false;
          endFlag[i] = true;
        }
      }
      if(endFlag[0] && endFlag[1]) {
        endFlag[0] = false;
        endFlag[1] = false;
        dataBTFlag = false;
      }
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
      // writeStringToDataBT(dataBT, str_buf);
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

//-----------------------------------------------

void setGoalAngle(int value) {
  setDirection(value);
  goalAngle[0] = value;
  goalAngle[1] = value;
}

void setDirection(int value) {
  if(value - goalAngle[0] > 0) {//right
    motor->setServoDirection(false);
  }
  else if(value - goalAngle[0] < 0) {//left
    motor->setServoDirection(true);
  }
}

bool swingJudge(int arrayNum) {
  int readValue = sensor->readPhotoValue(arrayNum);
  // Serial.println(readValue);
  beforPhotoFlag[arrayNum] = currentPhotoFlag[arrayNum];
  currentPhotoFlag[arrayNum] = (readValue > photoThreshold[arrayNum]) ? true : false;
  if(beforPhotoFlag[arrayNum] && !currentPhotoFlag[arrayNum]) {
    walkCount++;
    if(walkCount>=10) walkCount = 0;
    return true;
  }
  return false;
}

bool moveStopJudge(int arrayNum) {
  float sensorAngle = sensor->getDegValue(arrayNum);
  // Serial.println(sensorAngle);
  if(sensorAngle >= goalAngle[arrayNum] - 1 &&
  sensorAngle <= goalAngle[arrayNum] + 1) {
    return true;
  }
  return false;
}
