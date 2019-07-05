#include "BluetoothSerial.h"
#define BUFFER_SIZE 100
BluetoothSerial SerialBT;
uint8_t dataBT[BUFFER_SIZE];

#include "XL320.h"
XL320 xl320;
HardwareSerial Serial_default(0);//(RX, TX) (3, 1)
int servoID = 5;
bool commandFlag[2];
bool servoDirection;//true = CCW, flase = CW

#include <ESP32Servo.h>
Servo sg90;
int sg90Pin = A17;
int sg90Position = 0;

int senserPin[2] = {A0, A3};
bool countFlag;
int currentAngle;
int addAngle;
int goalAngle;

void init_all();
void init_xl320();
void init_sg90();
void init_sensor();

bool readSerialBT();
void writeStringToDataBT(uint8_t buffer[], String str);

void inputAngle(int value);
void updateGoalAngle(int value);
void countAngle();
bool moveStartSwitch();
void startServo();
void stopServo();


void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32");
  init_all();
}

void loop() {
  readSerialBT();
  countAngle();
  if(moveStartSwitch()) {
    if (addAngle != goalAngle) {
      if(commandFlag[0]) startServo();
    }
    else {
      if(commandFlag[1]) stopServo();
    }
  }
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
  commandFlag[0] = true;
  commandFlag[1] = false;
  servoDirection = true;
}
void init_sg90() {
  sg90.setPeriodHertz(50);
  sg90.attach(sg90Pin, 500, 2400);
  sg90.write(90);
}
void init_sensor() {
  int initValue = analogRead(senserPin[0]);
  currentAngle = 0;
  addAngle = 0;
  goalAngle = 0;
  if(initValue > 500) countFlag = true;
  else countFlag = false;
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
    SerialBT.write(dataBT, data_length);//write BluetoothSerial
  }
}

//-----------------------------------------------
//ctrl func

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

void countAngle() {
  int readValue = analogRead(senserPin[0]);
  if(!countFlag) {
    if(readValue > 500) {//white
      countFlag = true;
      addAngle += 1;
    }
  }
  else {
    if(readValue < 500) {//black
      countFlag = false;
      addAngle += 1;
    }
  }
  Serial.print("addAngle : ");
  Serial.print(addAngle);
  Serial.print(", goalAngle : ");
  Serial.println(goalAngle);
  // Serial.print("currentAngle : ");
  // Serial.println(currentAngle);
}
bool moveStartSwitch() {
  int readValue = analogRead(senserPin[1]);
  if(readValue > 500) return false;//touch ground
  else return true;
}
void startServo() {
  commandFlag[0] = false;
  commandFlag[1] = true;
  if(servoDirection) {
    sg90.write(90+45);
    delay(100);
    xl320.moveWheel(servoID, 1023);//CCW
    // xl320.moveWheel(servoID, 500);//CCW
  }
  else {
    sg90.write(90-45);
    delay(100);
    xl320.moveWheel(servoID, 2047);//CW
    // xl320.moveWheel(servoID, 1524);//CW
  }
}
void stopServo() {
  commandFlag[0] = true;
  commandFlag[1] = false;
  addAngle = 0;
  goalAngle = 0;
  xl320.moveWheel(servoID, 0);
  if(currentAngle > 0) {
    sg90.write(90+45);
  }
  else if(currentAngle < 0) {
    sg90.write(90-45);
  }
  else {
    sg90.write(90);
  }
}
