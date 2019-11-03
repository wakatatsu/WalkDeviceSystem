#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

#include "footSystem.hpp"

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
void printTest(OSCMessage &msg);
void setAngle(OSCMessage &msg);

//Pin[] = {PhotoSensor, RotaryEncoder, RotaryEncoder, SG90};
FootSystem* RightFoot;
const int RightPin[] = {32, 34, 35, 26};
FootSystem* LeftFoot;
const int LeftPin[] = {33, 36, 39, 25};

void setup() {
    Serial.begin(115200);
    // connectWiFi();
    SerialBT.begin("ESP32");
    Serial.println(F("RightFoot setup"));
    RightFoot = new FootSystem(RightPin, "right");
    Serial.println(F("LeftFoot setup"));
    LeftFoot = new FootSystem(LeftPin, "left");
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
    RightFoot->setAngle(value);
    LeftFoot->setAngle(value);
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
            message.dispatch("/AngleValue", setAngle);//read value to set goal angle
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

void setAngle(OSCMessage &msg) {
    int value = msg.getInt(0);
    RightFoot->setAngle(value);
    LeftFoot->setAngle(value);
}