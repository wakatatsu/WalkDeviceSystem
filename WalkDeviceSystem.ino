#include "BluetoothSerial.h"
#define BUFFER_SIZE 100
#define PHOTOSENSOR_THRESHOLD 500
#define SWITCH_THRESHOLD 300
#define INCREASE_ANGLE 2
//Bluetooth Serial
BluetoothSerial SerialBT;
uint8_t dataBT[BUFFER_SIZE];
bool dataBTFlag;
//XL320
#include "XL320.h"
XL320 xl320;
HardwareSerial Serial_default(0);//(RX, TX) (3, 1)
int servoID = 5;
bool servoDirection;//true = CCW, flase = CW
bool moveFlag;
//sg90
#include <ESP32Servo.h>
Servo sg90[2];
int sg90Pin[2] = {A16, A17};
int sg90Position[2];
//photo sensor and switch
int senserPin[2] = {A0, A3};
bool countFlag;
bool currentSwitchFlag;
bool beforSwitchFlag;
//angle
int currentAngle;
int addAngle;
int goalAngle;

void init_all();
void init_xl320();
void init_sg90();
void init_sensor();

bool readSerialBT();
void writeStringToDataBT(uint8_t buffer[], String str);

void countAngle();
void inputAngle(int value);
void updateGoalAngle(int value);
bool moveStartSwitch();
void startServo();
void stopServo();


void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32");
  dataBTFlag = false;
  init_all();
}

void loop() {
  if(readSerialBT() && !dataBTFlag) {
    dataBTFlag = true;
  }

  if(moveFlag) countAngle();
  if(moveStartSwitch() && dataBTFlag) {
    startServo();
    moveFlag = true;
  }
  if (moveFlag && (addAngle >= goalAngle)) {
    stopServo();
    moveFlag = false;
    dataBTFlag = false;
  }

  Serial.print("addAngle : ");
  Serial.print(addAngle);
  Serial.print(", goalAngle : ");
  Serial.println(goalAngle);
  // Serial.print("currentAngle : ");
  // Serial.println(currentAngle);
}

//-----------------------------------------------
//init func

void init_all() {
  init_xl320();
  init_sg90();
  init_sensor();
}
void init_xl320() {
  Serial_default.begin(115200);
  xl320.begin(Serial_default);
  xl320.TorqueON(servoID);
  xl320.LED(servoID, "g" );
  xl320.moveWheel(servoID, 0);
  servoDirection = true;
  moveFlag = false;
}
void init_sg90() {
  for(int i = 0; i < 2; i++) {
    sg90[i].setPeriodHertz(50);
    sg90[i].attach(sg90Pin[i], 500, 2400);
    sg90[i].write(90);
    sg90Position[i] = 0;
  }
}
void init_sensor() {
  int initValue = analogRead(senserPin[0]);
  currentAngle = 0;
  addAngle = 0;
  goalAngle = 0;
  if(initValue > 500) countFlag = true;
  else countFlag = false;
  currentSwitchFlag = false;
  beforSwitchFlag = false;
}

//-----------------------------------------------
//bluetoothSerial func

bool readSerialBT() {
  String str_buf;
  while(1) {
    if(SerialBT.available() > 0) {
      str_buf = SerialBT.readStringUntil('\n');//read BluetoothSerial buffer
      inputAngle(str_buf.toInt());
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

//-----------------------------------------------
//ctrl func

void countAngle() {
  int readValue = analogRead(senserPin[0]);
  if(!countFlag) {
    if(readValue > PHOTOSENSOR_THRESHOLD) {//white
      countFlag = true;
      addAngle += INCREASE_ANGLE;
    }
  }
  else {
    if(readValue < PHOTOSENSOR_THRESHOLD) {//black
      countFlag = false;
      addAngle += INCREASE_ANGLE;
    }
  }
}
void inputAngle(int value) {
  currentAngle += value;
  updateGoalAngle(value);
}
void updateGoalAngle(int value) {
  if(value > 0) {//right
    servoDirection = false;
  }
  else if(value < 0) {//left
    servoDirection = true;
    value *= -1;
  }
  goalAngle = value;
}
bool moveStartSwitch() {
  int readValue = analogRead(senserPin[1]);
  beforSwitchFlag = currentSwitchFlag;
  currentSwitchFlag = readValue > SWITCH_THRESHOLD ? true : false;
  if(beforSwitchFlag && !currentSwitchFlag)return true;
  return false;
}

//-----------------------------------------------
//motor func

void startServo() {
  if(servoDirection) {
    sg90[0].write(90-40);
    sg90[1].write(90+40);
    delay(200);
    xl320.moveWheel(servoID, 1023);//CCW
  }
  else {
    sg90[0].write(90+40);
    sg90[1].write(90-40);
    delay(200);
    xl320.moveWheel(servoID, 2047);//CW
  }
}
void stopServo() {
  xl320.moveWheel(servoID, 0);
  if(currentAngle > 0) {
    sg90[0].write(90-40);
    sg90[1].write(90+40);
  }
  else if(currentAngle < 0) {
    sg90[0].write(90+40);
    sg90[1].write(90-40);
  }
  else {
    sg90[0].write(90);
    sg90[1].write(90);
  }
  addAngle = 0;
  goalAngle = 0;
}
