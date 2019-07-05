#include <ESP32Servo.h>
Servo sg90;
int sg90Pin = A17;
int sg90Position = 0;

void init_sg90() {
  sg90.setPeriodHertz(50);
  sg90.attach(sg90Pin, 500, 2400);
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
  sg90.write(90);

}
