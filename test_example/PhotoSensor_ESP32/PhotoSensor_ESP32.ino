int readvalue;
float voltage;

void setup() {
  Serial.begin(115200);
}

void loop() {

  //read photo senser
  readvalue = analogRead(A0);
  voltage = (float)readvalue / 4096 * 3.3;
  Serial.print("readValue:");
  Serial.print(readvalue);
  Serial.print(", voltage:");
  Serial.println(voltage);

}
