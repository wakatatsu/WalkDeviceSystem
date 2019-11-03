#ifndef SENSOR_HPP_INCLUDE
#define SENSOR_HPP_INCLUDE

#define ROTARY_ENCODER_NUM 2
#define PHOTO_SENSOR_NUM 2
#define PCNT_H_LIM_VAL 100 //カウンタの上限
#define PCNT_L_LIM_VAL -100 //カウンタの下限

#include "Arduino.h"
#include "driver/pcnt.h"

class Sensor {

public:
  Sensor();
  int readPhotoValue(int num);
  int getCountValue(int num);
  float getDegValue(int num);

private:
  const int rotary_encoderPin[ROTARY_ENCODER_NUM*2] = {34, 36, 35, 39};
  int16_t count = 0;
  pcnt_config_t pcnt_config[ROTARY_ENCODER_NUM];
  const int photo_sensorPin[PHOTO_SENSOR_NUM] = {A4, A5};
  const float rotary_encoderDPC = 360 / 30;//deg per count of rotary encoder
  const float gearDPC = 360 / 18;//deg per count of gear
  const float rootDPC = 360 / 180;//deg per count of root

  void init_rotary_encoder();

};

#endif
