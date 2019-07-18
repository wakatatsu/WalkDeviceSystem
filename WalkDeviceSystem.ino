#include "BluetoothSerial.h"
#define BUFFER_SIZE 100
#define PHOTOSENSOR_THRESHOLD 100
#define SWITCH_THRESHOLD 4096/2
#define INCREASE_ANGLE 2
//Bluetooth Serial
BluetoothSerial SerialBT;
uint8_t dataBT[BUFFER_SIZE];//bluetooth data buffer
bool dataBTFlag[2];
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
int sensorPin[4] = {A0, A6, A3, A7};//{[0],[2]} = PotoSensor, {[1],[3]} = TacktSwitch
bool countFlag[2];
bool currentSwitchFlag[2];
bool beforSwitchFlag[2];
int countWalk;
//angle
int angleData[10] = {0,2,2,2,2,2,2,2,2,2};
int currentAngle;
int addAngle[2];
int goalAngle[2];

void init_all();
void init_xl320();
void init_sg90();
void init_sensor();

bool readSerialBT();
void writeStringToDataBT(uint8_t buffer[], String str);

void nextAngleData();
void inputAngle(int value);
void updateGoalAngle(int value);
void countAngle(int arrayNum);
bool moveStartSwitch(int arrayNum);
void startServo(int arrayNum);
void stopServo(int arrayNum);


void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32");
  dataBTFlag[0] = true;
  dataBTFlag[1] = true;
  init_all();
}

void loop() {
  for(int i = 0; i < 2; i++) {
    // if(dataBTFlag[0] == true && dataBTFlag[1] == true) {
    // nextAngleData();
    // }
    if(readSerialBT() && dataBTFlag[i]) {
      dataBTFlag[0] = false;
      dataBTFlag[1] = false;
    }
    if(moveFlag[i]) countAngle(i);
    if(moveStartSwitch(i) && !dataBTFlag[i]) {
      startServo(i);
      moveFlag[i] = true;
    }
    if (moveFlag[i] && (addAngle[i] >= goalAngle[i])) {
      stopServo(i);
      dataBTFlag[i] = true;
      moveFlag[i] = false;
    }
  }
  // Serial.print("addAngle : ");
  // Serial.print(addAngle[0]);
  // Serial.print(", goalAngle : ");
  // Serial.println(goalAngle[0]);
  // Serial.print("currentAngle : ");
  // Serial.println(currentAngle[1]);
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
    initValue = (i == 0) ? analogRead(sensorPin[i]) : analogRead(sensorPin[i+1]);
    countFlag[i] = (initValue > PHOTOSENSOR_THRESHOLD) ? true : false;//white:black
    currentSwitchFlag[i] = false;
    beforSwitchFlag[i] = false;
    addAngle[i] = 0;
    goalAngle[i] = 0;
  }
  currentAngle = 0;
  countWalk = 0;
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

void nextAngleData() {
  int value = angleData[countWalk/2];
  inputAngle(value);
  dataBTFlag[0] = false;
  dataBTFlag[1] = false;
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
  for(int i = 0; i < 2; i++) goalAngle[i] = value;
}
void countAngle(int arrayNum) {
  int readValue;
  readValue = (arrayNum == 0) ? analogRead(sensorPin[arrayNum]) : analogRead(sensorPin[arrayNum+1]);
  Serial.print("port ");
  Serial.print(arrayNum);
  Serial.print(", value = ");
  Serial.println(readValue);
  if(!countFlag[arrayNum]) {
    if(readValue > PHOTOSENSOR_THRESHOLD) {//white
      countFlag[arrayNum] = true;
      addAngle[arrayNum] += INCREASE_ANGLE;
    }
  }
  else {
    if(readValue < PHOTOSENSOR_THRESHOLD) {//black
      countFlag[arrayNum] = false;
      addAngle[arrayNum] += INCREASE_ANGLE;
    }
  }
}
bool moveStartSwitch(int arrayNum) {
  countWalk++;
  int readValue = (arrayNum == 0) ? analogRead(sensorPin[arrayNum+1]) : analogRead(sensorPin[arrayNum+2]);
  beforSwitchFlag[arrayNum] = currentSwitchFlag[arrayNum];
  currentSwitchFlag[arrayNum] = (readValue > SWITCH_THRESHOLD) ? true : false;
  return (beforSwitchFlag[arrayNum] && !currentSwitchFlag[arrayNum]) ? true : false;
}

//-----------------------------------------------
//motor func

void startServo(int arrayNum) {
  if(servoDirection) {
    sg90[arrayNum].write(90+40);
    sg90[arrayNum+2].write(90-40);
    delay(150);
    // xl320.moveWheel(servoID[arrayNum], 1023);//CCW
    xl320.moveWheel(servoID[arrayNum], 700);//CCW
    // xl320.moveWheel(servoID[arrayNum+2], 2047);//CW
    xl320.moveWheel(servoID[arrayNum+2], 1724);//CW
  }
  else {
    sg90[arrayNum].write(90-40);
    sg90[arrayNum+2].write(90+40);
    delay(150);
    // xl320.moveWheel(servoID[arrayNum], 2047);//CW
    xl320.moveWheel(servoID[arrayNum], 1724);//CW
    // xl320.moveWheel(servoID[arrayNum+2], 1023);//CCW
    xl320.moveWheel(servoID[arrayNum+2], 700);//CCW
  }
}
void stopServo(int arrayNum) {
  xl320.moveWheel(servoID[arrayNum], 0);
  xl320.moveWheel(servoID[arrayNum+2], 0);
  if(currentAngle > 0) {
    sg90[arrayNum].write(90+40);
    sg90[arrayNum+2].write(90-40);
  }
  else if(currentAngle < 0) {
    sg90[arrayNum].write(90-40);
    sg90[arrayNum+2].write(90+40);
  }
  else {
    sg90[arrayNum].write(90);
    sg90[arrayNum+2].write(90);
  }
  addAngle[arrayNum] = 0;
  goalAngle[arrayNum] = 0;
}
