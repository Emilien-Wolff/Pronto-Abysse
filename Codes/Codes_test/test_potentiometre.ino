#include <Wire.h> 

float sensorvalue;
int sensorpin = A0;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);    // Start serial communication at 115200 baud
  while(!Serial);
  Wire.setClock(400000);   // Set clock speed to be the fastest for better communication (fast mode)
  Serial.print("Current Device Address: ");

  analogReadResolution(12);
}

void loop() {
  // put your main code here, to run repeatedly:
  sensorvalue = analogRead(sensorpin)* 5.0 / 4096.;
  Serial.println(sensorvalue);
  delay(1000);
}
