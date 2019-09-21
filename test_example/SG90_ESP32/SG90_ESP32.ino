#include <ESP32Servo.h>
Servo sg90[4];
int sg90Pin[4] = {A4, A18, A5, A19};
int sg90Position = 0;
int value = 0;

void init_sg90() {
  for(int i = 0; i < 4; i++) {
    sg90[i].setPeriodHertz(50);
    sg90[i].attach(sg90Pin[i], 500, 2400);
  }
}

void setup() {
  Serial.begin(115200);
  init_sg90();
}

void loop() {

  //sg90
  // for (sg90Position = 0; sg90Position <= 180; sg90Position++) {
  //   sg90.write(sg90Position);
  //   delay(100);
  // }
  
  for(int i = 0; i < 2; i++) {
    sg90[i].write(90-40);
    sg90[i+2].write(90+40);
  }
  delay(1000);
  for(int i = 0; i < 4; i++) {
    sg90[i].write(90-40);
  }
  delay(1000);
  // value = digitalRead(23);
  // if(value > 0) {
  //   sg90[0].write(90);
  // }
  // else {
  //   sg90[0].write(0);
  // }
  // delay(100);

}
