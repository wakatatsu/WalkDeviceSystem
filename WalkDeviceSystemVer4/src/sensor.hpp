#ifndef SENSOR_HPP_INCLUDE
#define SENSOR_HPP_INCLUDE

#define PCNT_H_LIM_VAL 100 //カウンタの上限
#define PCNT_L_LIM_VAL -100 //カウンタの下限

#include "Arduino.h"
#include "driver/pcnt.h"

class Sensor {

public:
  Sensor(const int pins[], const char* footType);
  int readPhotoValue();
  int getCountValue();
  float getDegValue();
  bool setGoalAngle(int angle);
  bool swingJudge();
  bool moveStopJudge();

private:
  //foot type
  const char* type;
  //photo sensor
  const int photo_sensorPin;
  const int photoThreshold = 10;
  bool currentPhotoFlag = false;
  bool beforPhotoFlag = false;
  int currentAngle = 0;
  int goalAngle = 0;
  bool moveFlag = false;
  //rotary encoder
  const int rotary_encoderPin[2];
  pcnt_config_t pcnt_config;
  int16_t count = 0;
  const float rotary_encoderDPC = 360 / 30;//deg per count of rotary encoder
  const float gearDPC = 360 / 18;//deg per count of gear
  const float rootDPC = 360 / 180;//deg per count of root

  void init_rotary_encoder();

};

#endif
