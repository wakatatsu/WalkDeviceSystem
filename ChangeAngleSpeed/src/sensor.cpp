#include "sensor.hpp"

Sensor::Sensor(const int pins[], const char* footType) : type(footType),
 photo_sensorPin(pins[0]), rotary_encoderPin{pins[1], pins[2]} {
  init_rotary_encoder();
}

void Sensor::init_rotary_encoder() {
  pcnt_config.pulse_gpio_num = rotary_encoderPin[0];
  pcnt_config.ctrl_gpio_num = rotary_encoderPin[1];
  pcnt_config.lctrl_mode = PCNT_MODE_REVERSE;//制御ピンがLOWの時カウンタの振る舞い
  pcnt_config.hctrl_mode = PCNT_MODE_KEEP;//制御ピンがHIGHの時カウンタの振る舞い
  pcnt_config.channel = !strcmp(type, "right") ? PCNT_CHANNEL_0 : PCNT_CHANNEL_1;
  pcnt_config.unit = !strcmp(type, "right") ? PCNT_UNIT_0 : PCNT_UNIT_1;
  pcnt_config.pos_mode = !strcmp(type, "right") ? PCNT_COUNT_DEC : PCNT_COUNT_INC;//信号ピンに立ち上がりパルス(LOW->HIGH)が入った時どうするかの選択
  pcnt_config.neg_mode = !strcmp(type, "right") ? PCNT_COUNT_INC : PCNT_COUNT_DEC;//信号ピンに立ち下がりパルス(HIGH->LOW)が入った時どうするかの選択
  pcnt_config.counter_h_lim = PCNT_H_LIM_VAL;//カウンタ上限
  pcnt_config.counter_l_lim = PCNT_L_LIM_VAL;//カウンタ下限
  pcnt_unit_config(&pcnt_config);//ユニット初期化
  pcnt_counter_pause(!strcmp(type, "right") ? PCNT_UNIT_0 : PCNT_UNIT_1);//カウンタ一時停止
  pcnt_counter_clear(!strcmp(type, "right") ? PCNT_UNIT_0 : PCNT_UNIT_1);//カウンタ初期化
  pcnt_counter_resume(!strcmp(type, "right") ? PCNT_UNIT_0 : PCNT_UNIT_1);//カウント開始
}

int Sensor::readPhotoValue() {
  return analogRead(photo_sensorPin);
}

int Sensor::getCountValue() {
  pcnt_get_counter_value(!strcmp(type, "right") ? PCNT_UNIT_0 : PCNT_UNIT_1, &count);
  return count;
}

float Sensor::getDegValue() {
  return getCountValue() * rotary_encoderDPC / gearDPC * rootDPC;
}

bool Sensor::setGoalAngle(int angle) {
  bool direction = false;
  if(angle - goalAngle > 0) {//right
      direction = false;
  }
  else if(angle - goalAngle < 0) {//left
      direction = true;
  }
  goalAngle = angle;
  return direction;
}

int Sensor::getGoalAngle() {
  return goalAngle;
}

bool Sensor::swingJudge() {
  // int readValue = readPhotoValue();
  // // Serial.println(readValue);
  // beforPhotoFlag = currentPhotoFlag;
  // currentPhotoFlag = (readValue > photoThreshold) ? true : false;
  // if(beforPhotoFlag && !currentPhotoFlag) {
  //   return true;
  // }
  // return false;
  return true;
}

bool Sensor::moveStopJudge() {
  float sensorAngle = getDegValue();
  // Serial.println(sensorAngle);
  if(sensorAngle >= goalAngle - 1 &&
  sensorAngle <= goalAngle + 1) {
    return true;
  }
  return false;
}
