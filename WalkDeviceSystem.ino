#include "BluetoothSerial.h"
#define BUFFER_SIZE 100
#define PHOTOSENSOR_THRESHOLD 500
#define SWITCH_THRESHOLD 300
#define INCREASE_ANGLE 2
//Bluetooth Serial
BluetoothSerial SerialBT;
uint8_t dataBT[BUFFER_SIZE];//bluetooth data buffer
bool dataBTFlag;
//XL320
#include "XL320.h"
XL320 xl320;
HardwareSerial Serial_default(0);//(RX, TX) (3, 1)
int servoID[4] = {5, 7, 6, 8};
bool servoDirection;//true = CCW, false = CW
bool moveFlag[2];//[0] = right, [1] = left
//sg90
#include <ESP32Servo.h>
Servo sg90[4];
int sg90Pin[4] = {A4, A18, A5, A19};//{[0],[2]} = right, {[1],[3]} = left
//photo sensor and switch
int senserPin[4] = {A0, A3, A6, A7};//{[0],[2]} = PotoSensor, {[1],[3]} = TacktSwitch
bool countFlag[2];
bool currentSwitchFlag[2];
bool beforSwitchFlag[2];
//angle
int currentAngle[2];
int addAngle;
int goalAngle;

void init_all();
void init_xl320();
void init_sg90();
void init_sensor();

bool readSerialBT(int arrayNum);
void writeStringToDataBT(uint8_t buffer[], String str);

void countAngle(int arrayNum);
void inputAngle(int arrayNum, int value);
void updateGoalAngle(int value);
bool moveStartSwitch(int arrayNum);
void startServo(int arrayNum);
void stopServo(int arrayNum);


void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32");
  dataBTFlag = true;
  init_all();
}

void loop() {

  for(int i = 0; i < 2; i++) {
    if(readSerialBT(i) && dataBTFlag) {
      dataBTFlag = false;
    }
    if(moveFlag[i]) countAngle(i);
    if(moveStartSwitch(i) && !dataBTFlag) {
      startServo(i);
      moveFlag[i] = true;
    }
    if (moveFlag[i] && (addAngle >= goalAngle)) {
      stopServo(i);
      dataBTFlag = true;
      moveFlag[i] = false;
    }
  }

  // Serial.print("addAngle : ");
  // Serial.print(addAngle);
  // Serial.print(", goalAngle : ");
  // Serial.println(goalAngle);
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
  for(int i = 0; i < 4; i++) {
    xl320.TorqueON(servoID[i]);
    xl320.LED(servoID[i], "g" );
    xl320.moveWheel(servoID[i], 0);
  }
  servoDirection = true;
  for(int i = 0; i < 2; i++) moveFlag[i] = false;
}
void init_sg90() {
  for(int i = 0; i < 4; i++) {
    sg90[i].setPeriodHertz(50);
    sg90[i].attach(sg90Pin[i], 500, 2400);
    sg90[i].write(90);
  }
}
void init_sensor() {
  int initValue;
  for(int i = 0; i < 2; i++) {
    initValue = analogRead(senserPin[i]);
    if(initValue > PHOTOSENSOR_THRESHOLD) countFlag[i] = true;//white
    else countFlag[i] = false;//black
    currentAngle[i] = 0;
    currentSwitchFlag[i] = false;
    beforSwitchFlag[i] = false;
  }
  addAngle = 0;
  goalAngle = 0;
}

//-----------------------------------------------
//bluetoothSerial func

bool readSerialBT(int arrayNum) {
  String str_buf;
  while(1) {
    if(SerialBT.available() > 0) {
      str_buf = SerialBT.readStringUntil('\n');//read BluetoothSerial buffer
      inputAngle(arrayNum, str_buf.toInt());
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

void countAngle(int arrayNum) {
  int readValue = analogRead(senserPin[arrayNum]);
  if(!countFlag[arrayNum]) {
    if(readValue > PHOTOSENSOR_THRESHOLD) {//white
      countFlag[arrayNum] = true;
      addAngle += INCREASE_ANGLE;
    }
  }
  else {
    if(readValue < PHOTOSENSOR_THRESHOLD) {//black
      countFlag[arrayNum] = false;
      addAngle += INCREASE_ANGLE;
    }
  }
}
void inputAngle(int arrayNum, int value) {
  currentAngle[arrayNum] += value;
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
bool moveStartSwitch(int arrayNum) {
  int readValue = analogRead(senserPin[arrayNum+2]);
  beforSwitchFlag[arrayNum+2] = currentSwitchFlag[arrayNum+2];
  currentSwitchFlag[arrayNum+2] = readValue > SWITCH_THRESHOLD ? true : false;
  if(beforSwitchFlag[arrayNum+2] && !currentSwitchFlag[arrayNum+2])return true;
  return false;
}

//-----------------------------------------------
//motor func

void startServo(int arrayNum) {
  if(servoDirection) {
    sg90[arrayNum].write(90-40);
    sg90[arrayNum+2].write(90+40);
    delay(150);
    xl320.moveWheel(servoID[arrayNum], 1023);//CCW
    xl320.moveWheel(servoID[arrayNum+2], 2047);//CW
  }
  else {
    sg90[arrayNum].write(90+40);
    sg90[arrayNum+2].write(90-40);
    delay(150);
    xl320.moveWheel(servoID[arrayNum], 2047);//CW
    xl320.moveWheel(servoID[arrayNum+2], 1023);//CCW
  }
}
void stopServo(int arrayNum) {
  xl320.moveWheel(servoID[arrayNum], 0);
  xl320.moveWheel(servoID[arrayNum+2], 0);
  if(currentAngle > 0) {
    sg90[arrayNum].write(90-40);
    sg90[arrayNum+2].write(90+40);
  }
  else if(currentAngle < 0) {
    sg90[arrayNum].write(90+40);
    sg90[arrayNum+2].write(90-40);
  }
  else {
    sg90[arrayNum].write(90);
    sg90[arrayNum+2].write(90);
  }
  addAngle = 0;
  goalAngle = 0;
}
