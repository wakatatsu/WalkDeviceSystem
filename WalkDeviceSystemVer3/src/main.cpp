#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

#include "sensor.hpp"
#include "motor.hpp"

//-----------------------------------------------
//Bluetooth Serial
#include "BluetoothSerial.h"
#define BUFFER_SIZE 100
BluetoothSerial SerialBT;
uint8_t dataBT[BUFFER_SIZE];//bluetooth data buffer
bool readSerialBT();
void writeStringToDataBT(uint8_t buffer[], String str);
void setGoalAngle(int value);
//-----------------------------------------------

char ssid[] = "ESP32WiFi";          //your network SSID
char pass[] = "12345678";          //your network password

WiFiUDP Udp;
const IPAddress outIp(10,40,10,105);
const unsigned int outPort = 9999;
const unsigned int localPort = 8888;

OSCErrorCode error;
bool dataFlag = false;

void connectWiFi();
bool readWiFi();

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

void printTest(OSCMessage &msg);
void setGoalAngle(OSCMessage &msg);
void setDirection(int value);
bool swingJudge(int arrayNum);
bool moveStopJudge(int arrayNum);

void setup() {
  Serial.begin(115200);
  // connectWiFi();
  SerialBT.begin("ESP32");
  sensor = new Sensor();
  motor = new Motor();
}

void loop() {
  /*
  if(!dataFlag) {
    if(readWiFi()) {//set goal value if read
      Serial.println("read");
      dataFlag = true;
    }
  }
 */
 if(!dataFlag) {
    if(readSerialBT()) {//set goal value if read
      Serial.println("read");
      dataFlag = true;
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
        dataFlag = false;
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
void setGoalAngle(int value) {
  setDirection(value);
  goalAngle[0] = value;
  goalAngle[1] = value;
}
//-----------------------------------------------

void connectWiFi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
#ifdef ESP32
  Serial.println(localPort);
#else
  Serial.println(Udp.localPort());
#endif
}

bool readWiFi() {
  OSCMessage message;
  int size = Udp.parsePacket();//get size of packet
  if (size > 0) {
    while (size--) {
      message.fill(Udp.read());
    }
    if(!message.hasError()){
      message.dispatch("/SendTest", setGoalAngle);//read value to set goal angle
      // message.dispatch("/SendTest", printTest);//read value to set goal angle
    } else {
      error = message.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
    return true;
  }
  return false;
}

void printTest(OSCMessage &msg) {
  String txt = "value : ";
  int value = msg.getInt(0);
  char currentPlane[20];
  String str;
  msg.getString(1,currentPlane);
  txt += (String)value;
  str = currentPlane;
  txt += " str : " + str;
  Serial.println(txt);
}

void setGoalAngle(OSCMessage &msg) {
  int value = msg.getInt(0);
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