#include <Wire.h>

// Adresse I2C du MPU6050
const int MPU_addr = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int minVal = 265;
int maxVal = 402;

//sda sur a5,scl sur a4, gnd sur gnd, vcc sur 3.3v

double x;
double y;
double z;

void setup() {
  // Initialisation de la communication I2C
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); // Registre de configuration du MPU6050
  Wire.write(0); // Réveille le MPU6050
  Wire.endTransmission(true);

  // Initialisation de la communication série pour l'affichage des résultats
  Serial.begin(9600);
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

  // Affichage des résultats sur le moniteur série
  Serial.print("AngleX = ");
  Serial.println(x);
  Serial.print("AngleY = ");
  Serial.println(y);
  Serial.print("AngleZ = ");
  Serial.println(z);
  Serial.println("-----------------------------------------");

  delay(400); // Pause de 400 ms avant la prochaine lecture
}