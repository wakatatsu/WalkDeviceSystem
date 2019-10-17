int sensorPin[2] = {A4, A5};
int readValue[2];
float voltage;

void setup() {
  Serial.begin(115200);
}

void loop() {

  for(int i = 0; i < 2; i++) {
    readValue[i] = analogRead(sensorPin[i]);
  }
  Serial.print("PhotoSensor:");
  Serial.print(readValue[0]);
  Serial.print(", ");
  Serial.print(readValue[1]);
  Serial.println();
  delay(1);

  //read photo senser
  // readValue = analogRead(A0);
  // voltage = (float)readvalue / 4096 * 3.3;
  // Serial.print(value);

}
