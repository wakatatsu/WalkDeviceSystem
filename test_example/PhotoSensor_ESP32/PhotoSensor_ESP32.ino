int sensorPin[4] = {A0, A6, A3, A7};//{[0],[2]} = PotoSensor, {[1],[3]} = TacktSwitch
int readValue[4];
float voltage;

void setup() {
  Serial.begin(115200);
}

void loop() {

  for(int i = 0; i < 2; i++) {
    readValue[i] = analogRead(sensorPin[i]);
    readValue[i+2] = analogRead(sensorPin[i+2]);
  }
  Serial.print("PhotoSensor:");
  Serial.print(readValue[0]);
  Serial.print(", ");
  Serial.print(readValue[2]);
  Serial.print("  SwitchSensor:");
  Serial.print(readValue[1]);
  Serial.print(", ");
  Serial.println(readValue[3]);
  delay(1);

  //read photo senser
  // readValue = analogRead(A0);
  // voltage = (float)readvalue / 4096 * 3.3;


  // Serial.print("readValue:");
  // Serial.print(readvalue);
  // Serial.print(", voltage:");
  // Serial.println(voltage);

}
