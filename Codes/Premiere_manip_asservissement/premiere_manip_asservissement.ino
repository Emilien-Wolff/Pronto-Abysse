#include <Wire.h>


//setup mpu
// Adresse I2C du MPU6050
const int MPU_addr = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int minVal = 265;
int maxVal = 402;

//sda sur a5,scl sur a4, gnd sur gnd, vcc sur 3.3v

double x;
double y;
double z;


//setup driver 
//Ports de commande du moteur B
int motorPin1 = 8;
int motorPin2 = 9;
int enablePin = 5;
float sensorvalue1;
int sensorpin1 = A0; 
float sensorvalue2;
int sensorpin2 = A1;

// Vitesse du moteur
int state = 0;

void setup() {
    // Configuration des ports en mode "sortie"
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(sensorpin1, INPUT);
  pinMode(sensorpin2, INPUT);


  // Initialisation de la communication I2C
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); // Registre de configuration du MPU6050
  Wire.write(0); // Réveille le MPU6050
  Wire.endTransmission(true);

  // Initialisation de la communication série pour l'affichage des résultats
  Serial.begin(9600);

  analogReadResolution(12);
}

void loop() {
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

  x = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI);
  y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
  z = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);

  if (y>30 && y<180){
    state = 100;
  }
  else if (y<330 && y>180){
    state= -100;
  }
  else{
    state = 0;
  }

      if (state > 0) // avant
    {
      digitalWrite(motorPin1, HIGH); 
      digitalWrite(motorPin2, LOW);
      Serial.print("Avant ");
      Serial.println(state);
    }
    else if (state < 0) // arrière
    {
      digitalWrite(motorPin1, LOW); 
      digitalWrite(motorPin2, HIGH);
      Serial.print("Arriere ");
      Serial.println(state);
    }
    else // Stop (freinage)
    {
      digitalWrite(motorPin1, HIGH); 
      digitalWrite(motorPin2, HIGH);
      Serial.println("Stop");
    }

      //
      // Vitesse du mouvement
      //
    analogWrite(enablePin, abs(state));
    delay(100);
}