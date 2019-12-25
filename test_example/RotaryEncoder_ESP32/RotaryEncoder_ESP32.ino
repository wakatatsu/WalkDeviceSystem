/*
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
*/
#include "driver/pcnt.h"

void qei_setup_x4(pcnt_unit_t pcnt_unit, int gpioA, int gpioB)
{
  pcnt_config_t pcnt_confA;
  pcnt_config_t pcnt_confB;

  pcnt_confA.unit = pcnt_unit;
  pcnt_confA.channel = PCNT_CHANNEL_0;
  pcnt_confA.pulse_gpio_num = gpioA;
  pcnt_confA.ctrl_gpio_num = gpioB;
  pcnt_confA.pos_mode = PCNT_COUNT_INC;
  pcnt_confA.neg_mode = PCNT_COUNT_DEC;
  pcnt_confA.lctrl_mode = PCNT_MODE_REVERSE;
  pcnt_confA.hctrl_mode = PCNT_MODE_KEEP;
  pcnt_confA.counter_h_lim = 32767;
  pcnt_confA.counter_l_lim = -32768;

  pcnt_confB.unit = pcnt_unit;
  pcnt_confB.channel = PCNT_CHANNEL_1;
  pcnt_confB.pulse_gpio_num = gpioB;
  pcnt_confB.ctrl_gpio_num = gpioA;
  pcnt_confB.pos_mode = PCNT_COUNT_INC;
  pcnt_confB.neg_mode = PCNT_COUNT_DEC;
  pcnt_confB.lctrl_mode = PCNT_MODE_KEEP;
  pcnt_confB.hctrl_mode = PCNT_MODE_REVERSE;
  pcnt_confB.counter_h_lim = 32767;
  pcnt_confB.counter_l_lim = -32768;

  /* Initialize PCNT unit */
  pcnt_unit_config(&pcnt_confA);
  pcnt_unit_config(&pcnt_confB);

  pcnt_counter_pause(pcnt_unit);
  pcnt_counter_clear(pcnt_unit);
  pcnt_counter_resume(pcnt_unit);
}

void qei_setup_x1(pcnt_unit_t pcnt_unit, int gpioA, int gpioB)
{
  pcnt_config_t pcnt_confA;

  pcnt_confA.unit = pcnt_unit;
  pcnt_confA.channel = PCNT_CHANNEL_0;
  pcnt_confA.pulse_gpio_num = gpioA;
  pcnt_confA.ctrl_gpio_num = gpioB;
  pcnt_confA.pos_mode = PCNT_COUNT_INC;      //立ち上がりのみカウント
  pcnt_confA.neg_mode = PCNT_COUNT_DIS;      //立ち下がりはカウントしない
  pcnt_confA.lctrl_mode = PCNT_MODE_REVERSE; //立ち上がり時にB相がHighなら逆転
  pcnt_confA.hctrl_mode = PCNT_MODE_KEEP;
  pcnt_confA.counter_h_lim = 32767;//max 32767;
  pcnt_confA.counter_l_lim = -32767;//min -32768;

  /* Initialize PCNT unit */
  pcnt_unit_config(&pcnt_confA);
  pcnt_set_filter_value(pcnt_unit, 1023);//誤認防止フィルタ


  pcnt_counter_pause(pcnt_unit);
  pcnt_counter_clear(pcnt_unit);
  pcnt_counter_resume(pcnt_unit);
}

void test(pcnt_unit_t pcnt_unit, int gpioA, int gpioB) {
  pcnt_config_t pcnt_config;
  
  pcnt_config.pulse_gpio_num = gpioA;
  pcnt_config.ctrl_gpio_num = gpioB;
  pcnt_config.lctrl_mode = PCNT_MODE_REVERSE;//制御ピンがLOWの時カウンタの振る舞い
  pcnt_config.hctrl_mode = PCNT_MODE_KEEP;//制御ピンがHIGHの時カウンタの振る舞い
  pcnt_config.channel = PCNT_CHANNEL_0;
  pcnt_config.unit = pcnt_unit;
  pcnt_config.pos_mode = PCNT_COUNT_INC;//信号ピンに立ち上がりパルス(LOW->HIGH)が入った時どうするかの選択
  pcnt_config.neg_mode = PCNT_COUNT_DEC;//信号ピンに立ち下がりパルス(HIGH->LOW)が入った時どうするかの選択
  pcnt_config.counter_h_lim = 1000;//カウンタ上限
  pcnt_config.counter_l_lim = -1000;//カウンタ下限
  
  pcnt_unit_config(&pcnt_config);//ユニット初期化
  pcnt_set_filter_value(pcnt_unit, 1023);//誤認防止フィルタ
  pcnt_counter_pause(pcnt_unit);//カウンタ一時停止
  pcnt_counter_clear(pcnt_unit);//カウンタ初期化
  pcnt_counter_resume(pcnt_unit);//カウント開始
}

void setup()
{
    Serial.begin(115200);
//    qei_setup_x4(PCNT_UNIT_2, GPIO_NUM_35, GPIO_NUM_34); //LEFT
//    qei_setup_x4(PCNT_UNIT_3, GPIO_NUM_39, GPIO_NUM_36); //RIGHT
//    pinMode(A5, INPUT);
    test(PCNT_UNIT_2, GPIO_NUM_35, GPIO_NUM_34); //LEFT
    test(PCNT_UNIT_3, GPIO_NUM_39, GPIO_NUM_36); //RIGHT
    delay(5000);
}

void loop()
{
    int16_t count_L;
    int16_t count_R;
    pcnt_get_counter_value(PCNT_UNIT_2, &count_L);
    pcnt_get_counter_value(PCNT_UNIT_3, &count_R);

    Serial.print(" Pulse L:");
    Serial.print(count_L);
    Serial.print(" R:");
    Serial.println(count_R);
}
