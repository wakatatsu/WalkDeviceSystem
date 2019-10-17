#include "driver/pcnt.h"

#define PULSE_INPUT_PIN 36 //パルスの入力ピン 今回はエンコーダのA相を接続
#define PULSE_CTRL_PIN 39 //制御ピン 今回はエンコーダのB相を接続
#define PCNT_H_LIM_VAL 100 //カウンタの上限 今回は使ってない
#define PCNT_L_LIM_VAL -100 //カウンタの下限 今回は使ってない

int16_t count = 0; //カウント数

void setup() {
  pcnt_config_t pcnt_config;//設定用の構造体の宣言
        pcnt_config.pulse_gpio_num = PULSE_INPUT_PIN;
        pcnt_config.ctrl_gpio_num = PULSE_CTRL_PIN;
        pcnt_config.lctrl_mode = PCNT_MODE_REVERSE;
        pcnt_config.hctrl_mode = PCNT_MODE_KEEP;
        pcnt_config.channel = PCNT_CHANNEL_0;
        pcnt_config.unit = PCNT_UNIT_0;
        pcnt_config.pos_mode = PCNT_COUNT_INC;
        pcnt_config.neg_mode = PCNT_COUNT_DEC;;
        pcnt_config.counter_h_lim = PCNT_H_LIM_VAL;
        pcnt_config.counter_l_lim = PCNT_L_LIM_VAL;
  pcnt_unit_config(&pcnt_config);//ユニット初期化
  pcnt_set_filter_value(PCNT_UNIT_0, 1023);//誤認防止フィルタ
  pcnt_counter_pause(PCNT_UNIT_0);//カウンタ一時停止
  pcnt_counter_clear(PCNT_UNIT_0);//カウンタ初期化
  Serial.begin(115200);
  pcnt_counter_resume(PCNT_UNIT_0);//カウント開始
}

void loop() {
  pcnt_get_counter_value(PCNT_UNIT_0, &count);
  if(count > 100) pcnt_counter_clear(PCNT_UNIT_0);
  if(count < -100) pcnt_counter_clear(PCNT_UNIT_0);
  Serial.print("Counter value: ");
  Serial.println(count);
  delay(1);
}
