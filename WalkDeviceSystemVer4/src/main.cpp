#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

#include "footSystem.hpp"

const int commuPin = 14;
int commuFlag = 0;//0 = wifi, 1 = bluetooth
const int debugPin[] = {12, 13};//{OUTPUT, INPUT}
int debugFlag = 0;
void debugFunc();

//-----------------------------------------------
//Bluetooth Serial
#include "BluetoothSerial.h"
#define BUFFER_SIZE 100
BluetoothSerial SerialBT;
uint8_t dataBT[BUFFER_SIZE];//bluetooth data buffer
bool readSerialBT();
void writeStringToDataBT(uint8_t buffer[], String str);
void setAngle(int value);
//-----------------------------------------------
//WiFi setting
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
bool dataFlag = false;

void connectWiFi();
bool readWiFi();
void printTest(OSCMessage &msg);
void setAngle(OSCMessage &msg);

//Pin[] = {PhotoSensor, RotaryEncoderA, RotaryEncoderB, SG90};
FootSystem* RightFoot;
const int RightPin[] = {A4, 34, 35, 26};
FootSystem* LeftFoot;
const int LeftPin[] = {A5, 36, 39, 25};

// void test(pcnt_unit_t pcnt_unit, int pinA, int pinB);

void setup() {
    Serial.begin(115200);

    pinMode(debugPin[0], OUTPUT);
    pinMode(debugPin[1], INPUT);
    digitalWrite(debugPin[0], 0);
    debugFlag = digitalRead(debugPin[1]);
    commuFlag = digitalRead(commuPin);
    
    if(!debugFlag) {
        if(commuFlag) {
            SerialBT.begin("ESP32");
            Serial.println("begin Bluetooth");
        }
        else connectWiFi();
    }

    //right = 0, left = 1
    Serial.println(F("RightFoot setup"));
    RightFoot = new FootSystem(RightPin, 0);
    Serial.println(F("LeftFoot setup"));
    LeftFoot = new FootSystem(LeftPin, 1);

    while(debugFlag) debugFunc();

    Serial.println(F("Start loop"));
}

void loop() {
    if(!dataFlag) {
        if(commuFlag) {
            if(readSerialBT()) {//set goal value if read
                Serial.println("read");
            }
        }
        else {
            if(readWiFi()) {//set goal value if read
                Serial.println("read");
            }
        }
    }
    else {
        RightFoot->run();
        LeftFoot->run();
        if(RightFoot->getEndFlag() &&
         LeftFoot->getEndFlag()) {
            dataFlag = false;
            RightFoot->setEndFlag(false);
            LeftFoot->setEndFlag(false);
        }
    }
    
}

//-----------------------------------------------
//debug func
void debugFunc() {
    Serial.println(F("RightFoot Debug"));
    Serial.println(F("waite 5s"));
    delay(5000);
    RightFoot->runDebug();

    Serial.println(F("LeftFoot Debug"));
    Serial.println(F("waite 5s"));
    delay(5000);
    LeftFoot->runDebug();
}


//-----------------------------------------------
//bluetoothSerial func
bool readSerialBT() {
    String str_buf;
    while(1) {
        if(SerialBT.available() > 0) {
            str_buf = SerialBT.readStringUntil('\n');//read BluetoothSerial buffer
            setAngle(str_buf.toInt());
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
void setAngle(int value) {
    dataFlag = RightFoot->setAngle(value) && LeftFoot->setAngle(value) ?
     true : false;
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
            message.dispatch("/angle", setAngle);//read value to set goal angle
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

//-----------------------------------------------
//call in readWiFi
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

void setAngle(OSCMessage &msg) {
    int value = msg.getInt(0);
    dataFlag = RightFoot->setAngle(value) && LeftFoot->setAngle(value) ?
     true : false;
}