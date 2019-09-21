#include "sensor.hpp"

Sensor::Sensor() {
  init_rotary_encoder();
}

void Sensor::init_rotary_encoder() {
  for(int i = 0; i < 2; i++) {
    pcnt_config[i].pulse_gpio_num = rotary_encoderPin[i];
    pcnt_config[i].ctrl_gpio_num = rotary_encoderPin[i+2];
    pcnt_config[i].lctrl_mode = PCNT_MODE_REVERSE;//制御ピンがLOWの時カウンタの振る舞い
    pcnt_config[i].hctrl_mode = PCNT_MODE_KEEP;//制御ピンがHIGHの時カウンタの振る舞い
    pcnt_config[i].channel = (i==0) ? PCNT_CHANNEL_0 : PCNT_CHANNEL_1;
    pcnt_config[i].unit = (i==0) ? PCNT_UNIT_0 : PCNT_UNIT_1;
    pcnt_config[i].pos_mode = PCNT_COUNT_INC;//信号ピンに立ち上がりパルス(LOW->HIGH)が入った時どうするかの選択
    pcnt_config[i].neg_mode = PCNT_COUNT_DEC;//信号ピンに立ち下がりパルス(HIGH->LOW)が入った時どうするかの選択
    pcnt_config[i].counter_h_lim = PCNT_H_LIM_VAL;//カウンタ上限
    pcnt_config[i].counter_l_lim = PCNT_L_LIM_VAL;//カウンタ下限
    pcnt_unit_config(&pcnt_config[i]);//ユニット初期化
    pcnt_counter_pause((i==0) ? PCNT_UNIT_0 : PCNT_UNIT_1);//カウンタ一時停止
    pcnt_counter_clear((i==0) ? PCNT_UNIT_0 : PCNT_UNIT_1);//カウンタ初期化
    pcnt_counter_resume((i==0) ? PCNT_UNIT_0 : PCNT_UNIT_1);//カウント開始
  }
}

int Sensor::readPhotoValue(int num) {
  return analogRead(photo_sensorPin[num]);
}

int Sensor::getCountValue(int num) {
  pcnt_get_counter_value((num==0) ? PCNT_UNIT_0 : PCNT_UNIT_1, &count);
  return count;
}

int Sensor::getDegValue(int num) {
  return dpc * getCountValue(num);
}
