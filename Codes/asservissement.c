/*
This file deals with the ballast part of the submarine.
@author : Gautier Pauliat
@version : 1.0
*/

#include <Wire.h>
#include "asservissement.h"


// Adresse I2C du MPU6050
const int MPU_addr = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int minVal = 265;
int maxVal = 402;


//Initialisation for the sensors to read the filling percentage
//float sensor_ballast_front_value;
const int sensor_ballast_front = A0; //potentiometre pin for the front ballast : this isn't the correct pin 

const int sensor_ballast_back = A1; //potentiometre pin for the front ballast : this isn't the correct pin
//float sensor_ballast_back_value;



void setup() {
  // Initialisation of the I2C communication
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); //MPU6050 configuration registory  
  Wire.write(0); // wake the MPU6050 up
  Wire.endTransmission(true);

  // Initialisation de la communication série pour l'affichage des résultats
  Serial.begin(9600); //Might not be the correct value
  //Wire.setClock(400000);   // Set clock speed to be the fastest for better communication (fast mode)
  analogReadResolution(12);
}


//Ports de commande du moteur B
int motorPin1 = 8;
int motorPin2 = 9;
int enablePin = 5;
float sensorvalue1;
int sensorpin1 = A0; 
float sensorvalue2;
int sensorpin2 = A1;

void setup() {
    // Configuration des ports en mode "sortie"
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(enablePin, OUTPUT);
    pinMode(sensorpin1, INPUT);
    pinMode(sensorpin2, INPUT);
}    

void fill_ballast(bool front, float filling_percentage){
  
  int abs_speed = 250;
  int speed;
  int motor_pin1, motor_pin2;


  if (front){
    moteur_pin1, moteur_pin2 = 8, 9; //these are not the correct pin number
  }
  else{
    motor_pin1, motor_pin2 = 10, 11; //these are not the correct pin number
  }

  float current_percentage = read_ballast(true);
  if (current_percentage> filling_percentage){ // le ballast est trop rempli
    speed = -abs_speed //maybe it's a +, we need to test
  }

  elif (pourcentage_actuel < pourcentage){
    speed = abs_speed // maybe it's a -
  }

  if (speed > 0) // forward
    {
      digitalWrite(moteur_pin1, HIGH); 
      digitalWrite(moteur_pin2, LOW);
    }
    else if (speed < 0) // backward
    {
      digitalWrite(moteur_pin1, LOW); 
      digitalWrite(moteur_pin2, HIGH);
    }
    else // Stop (braking)
    {
      digitalWrite(moteur_pin1, HIGH); 
      digitalWrite(moteur_pin2, HIGH);
    }

      //
      // Movement speed
      //
    analogWrite(enablePin, abs_speed);



}


int read_angles(){
  /*
  This function returns the current horizontal angle of the submarine with the ground
  */

  // Demande de lecture des données des capteurs du MPU6050
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B); // Registre de début des données de capteur
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // Lecture des 14 registres

  // Lecture des valeurs de l'accéléromètre et du gyroscope
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  // Calcul des angles de rotation
  int xAng = map(AcX, minVal, maxVal, -90, 90);
  int yAng = map(AcY, minVal, maxVal, -90, 90);
  int zAng = map(AcZ, minVal, maxVal, -90, 90);

  return yAng; //maybe we should return xAng or zAng, idk yet
}

float ideal_percentage(bool front, int angle){
  /*
  This function returns the ideal filling percentage for the ballast
  for the submarine to be horizontal to the ground 
  Input : 
    - bool front : indicates if we want to work with the front or the back ballast
    - int angle : current angle 
  Output : 
    - None
  Consequences : correction of the angle
  */

  return float(abs(angle)) / 100; //The formula needs adjustment

  //Maybe we can try to have the same water volume as before. We would need to empty/fill the other ballast
  //as well
}

float read_ballast(bool front){
  
  //Returns the filling percentage of selected ballast (front or back)
  //by reading the corresponding potentiometre value
  float sensorvalue;
  if (front){
    sensorvalue = analogRead(sensor_ballast_front) * arduino_tension / 4096.;
  }
  else{
    sensorvalue = analogRead(sensor_ballast_back) * arduino_tension / 4096.;
  }
  
  return sensorvalue;
}

void correct_angle(){
  /*
  This function corrects the filling percentages of the ballast 
  if needed, in order to have a horizontal angle with the ground 
  close to zero
  */
  int angle = read_angles();

  int threshold = 10; //angle tolerance
  float current_percentage = read_ballast(true);
  if (angle > threshold){ //We need to fill the front ballast (or back one idk)
    fill_ballast(true, ideal_percentage(true, angle));
  }
  else if (angle < threshold){
    angle = read_ballast(false);
    fill_ballast(false, ideal_percentage(false, angle));
  }
  // |angle| < threshold, nothing to do

}

