#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SCK 18
#define MISO 19
#define MOSI 17
#define SS 5

File dataFile;
String fileName = "/data.csv";
File dataFiles[3];
String fileNames[3] = {"/dx.csv", "/dy.csv", "/dz.csv"};



char ssid[] = "ESP32WiFi";          //your network SSID
char pass[] = "12345678";          //your network password
IPAddress ip(192, 168, 1, 3);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiUDP Udp;
const IPAddress outIp(10,40,10,105);
const unsigned int outPort = 9999;
const unsigned int localPort = 8888;

OSCErrorCode error;

void connectWiFi();
bool readWiFi();
void positionWriteX(OSCMessage &msg);
void positionWriteY(OSCMessage &msg);
void positionWriteZ(OSCMessage &msg);
void positionWrite(String type, OSCMessage &msg);

int count = 0;
float value = 0.0;
void testWrite(int num, float value);

void setup() {
  Serial.begin(115200);

  pinMode(16, OUTPUT);
  pinMode(22, INPUT);

  SPI.end();
  SPI.begin(SCK, MISO, MOSI, SS); 
  if(!SD.begin(SS, SPI)){
      Serial.println("Card Mount Failed");
      return;
  }

  for(int i = 0; i < 3; i++) {
    dataFiles[i] = SD.open(fileNames[i], FILE_WRITE);
      if(!dataFiles[i]) {
      Serial.println(fileNames[i] + " : open file error");
      while(1);
    }
    Serial.println(fileNames[i] + " open");
  }
  

  connectWiFi();

  digitalWrite(16, 1);

}

void loop() {

  // testWrite(count, value);  
  // count++;
  // value += 0.1;

  if(readWiFi()) Serial.println("read");

  if(digitalRead(22)) {
    dataFile.close();
    for(int i = 0; i < 3; i++) dataFiles[i].close();
    digitalWrite(16, 0);
    Serial.println("File close");
    while(1);
  }

}

//-----------------------------------------------
//Wifi func
void connectWiFi() {
    WiFi.config(ip, gateway, subnet); 
    delay(10);
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
            message.dispatch("/posiX", positionWriteX);
            message.dispatch("/posiY", positionWriteY);
            message.dispatch("/posiZ", positionWriteZ);
        } else {
            error = message.getError();
            Serial.print("error: ");
            Serial.println(error);
        }
        return true;
    }
    return false;
}

//-----------------------------------------------
//call in readWiFi
void positionWriteX(OSCMessage &msg) {
  positionWrite("x", msg);
}

void positionWriteY(OSCMessage &msg) {
  positionWrite("y", msg);
}

void positionWriteZ(OSCMessage &msg) {
  positionWrite("z", msg);
}

void positionWrite(String type, OSCMessage &msg) {
  String txt = "";
  int time = msg.getInt(0);
  float position = msg.getFloat(1);
  txt += (String)time + ",";
  txt += (String)position;
  
  if(type == "x") dataFiles[0].println(txt);
  else if(type == "y") dataFiles[1].println(txt);
  else if(type == "z") dataFiles[2].println(txt);
  else Serial.println("error");

}

//-----------------------------------------------
void testWrite(int num, float value) {
  String txt = "";
  txt += (String)num + ",";
  txt += (String)value;
  dataFile.println(txt);
}