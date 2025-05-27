/* Sweep */

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
Servo myservo1;
// twelve servo objects can be created on most boards

int pos = 0;

void setup() {
  myservo.attach(10);  // attaches the servo on pin 9 to the servo object
  myservo1.attach(9);
  Serial.begin(9600);
}

void loop() {
  Serial.println(myservo.read());
  myservo.write(0);
  myservo1.write(0);
  delay(2000);
  myservo.write(90);
  delay(2000);
  myservo.write(180);
  delay(2000);
}

