#include <ESP32Servo.h>
Servo sg90[2];
int sg90Pin[2] = {A19, A18};
int sg90Position = 0;
int value = 0;

void init_sg90() {
  for(int i = 0; i < 2; i++) {
    sg90[i].setPeriodHertz(50);
    sg90[i].attach(sg90Pin[i], 500, 2400);
    sg90[i].write(180);
  }
  delay(100);
}

void setup() {
  Serial.begin(115200);
  init_sg90();
}

void loop() {

  for(int i = 0; i < 2; i++) {
    sg90[i].write(90);
  }
  delay(2000);
  for(int i = 0; i < 2; i++) {
    sg90[i].write(180);
  }
  delay(2000);

}
