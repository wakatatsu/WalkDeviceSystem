#include "sensor.hpp"

Sensor::Sensor()  : type(0),photo_sensorPin(0), rotary_encoderPin{0, 0} {
  
}

Sensor::Sensor(const int pins[], int footType) : type(footType),
 photo_sensorPin(pins[0]), rotary_encoderPin{pins[1], pins[2]} {
   init_photo_sensor(photo_sensorPin);
   init_rotary_encoder((type == 0) ? PCNT_UNIT_0 : PCNT_UNIT_1,
      rotary_encoderPin[0], rotary_encoderPin[1]);
}

void Sensor::init_photo_sensor(int pin) {
  pinMode(pin, INPUT);
}

void Sensor::init_rotary_encoder(pcnt_unit_t pcnt_unit, int pinA, int pinB) {
  pcnt_config_t pcnt_config;

  pcnt_config.pulse_gpio_num = pinA;
  pcnt_config.ctrl_gpio_num = pinB;
  pcnt_config.lctrl_mode = PCNT_MODE_REVERSE;//制御ピンがLOWの時カウンタの振る舞い
  pcnt_config.hctrl_mode = PCNT_MODE_KEEP;//制御ピンがHIGHの時カウンタの振る舞い
  pcnt_config.channel = PCNT_CHANNEL_0;
  pcnt_config.unit = pcnt_unit;
  pcnt_config.pos_mode = (type == 0) ? PCNT_COUNT_DEC : PCNT_COUNT_INC;//信号ピンに立ち上がりパルス(LOW->HIGH)が入った時どうするかの選択
  pcnt_config.neg_mode = (type == 0) ? PCNT_COUNT_INC : PCNT_COUNT_DEC;//信号ピンに立ち下がりパルス(HIGH->LOW)が入った時どうするかの選択
  pcnt_config.counter_h_lim = PCNT_H_LIM_VAL;//カウンタ上限
  pcnt_config.counter_l_lim = PCNT_L_LIM_VAL;//カウンタ下限
  pcnt_unit_config(&pcnt_config);//ユニット初期化
  pcnt_set_filter_value(pcnt_unit, 1023);//誤認防止フィルタ
  pcnt_filter_enable(pcnt_unit);
  pcnt_counter_pause(pcnt_unit);//カウンタ一時停止
  pcnt_counter_clear(pcnt_unit);//カウンタ初期化
  pcnt_counter_resume(pcnt_unit);//カウント開始
  
}

int Sensor::readPhotoValue() {
  return analogRead(photo_sensorPin);
}

int Sensor::getCountValue() {
  int16_t count;
  pcnt_get_counter_value((type == 0) ? PCNT_UNIT_0 : PCNT_UNIT_1, &count);
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
  int readValue = readPhotoValue();
  // Serial.println(readValue);
  beforPhotoFlag = currentPhotoFlag;
  currentPhotoFlag = (readValue > photoThreshold) ? true : false;
  if(beforPhotoFlag && !currentPhotoFlag) {
    return true;
  }
  return false;
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