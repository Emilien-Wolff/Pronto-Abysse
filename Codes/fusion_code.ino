#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"
#include "Servo.h"
#include "ADS1X15.h"
#include "Serial.h"

// -------------------------ADS1115 variables -------------------------------

ADS1115 ADS(0x48);  // ADS1115 physiquement défini à l'adresse 0x48, avec sa broche ADDR reliée à la masse

int16_t tension_A0;
int16_t tension_A1;
float tension_volts_A0; //tension en volts du premier potard (normalement affilié au moteur 0)
float tension_volts_A1; //tension en volts du deuxième potard (normalement affilié au moteur 1)
// --------------------------------------------------------------------------

// ----------------------MPU6050 variables-------------------------------------
MPU6050 mpu;

// Adresse I2C du MPU6050
const int MPU_addr = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int minVal = 265;
int maxVal = 402;

//sda sur a5,scl sur a4, gnd sur gnd, vcc sur 3.3v

double x;
double y;
double z;

// ---------------------------------------------------------------------------

// ---------------------DRIVER MINIMOTEURS VARIABLES----------------------------
//Ports de commande du moteur A (moteur avant)
int motorPinA1 = 8;
int motorPinA2 = 7;
int enablePinA = 6;
//Ports de commande du moteur B (moteur arrière)
int motorPinB1 = 4;
int motorPinB2 = 2;
int enablePinB = 3;
 
// Vitesse du moteur A
int stateA = 0;   //entre -255 et 255
// Vitesse du moteur B
int stateB = 0;   //entre -255 et 255

int working_range = 255;
// ----------------------------------------------------------------------------------

// --------------------------------SERVO variables--------------------------------------
//initialisation des servos
Servo servo1;
Servo servo2;

int limite_hori = 255;
int limite_vert = 255;

//----------------------------------------------------------------------------------

// ----------------------------------RADIO variables-------------------------------

int pin_barre_hori = A0;      // pin contrôlant l'inclinaison des gouvernails verticaux
int pin_barre_vert = A1;      // pin contrôlant l'inclinaison des gouvernails horizontaux
int pin_plonge = A2;          // pin contrôlant la plonge
int barre_vert;               // valeur récupérées
int barre_hori;               
int plonge;



int res_min = 950;              // longueur de signal min du pwm
int res_max = 1880;           // longueur de signal max du pwm

// --------------------------------------------------------------------------------

/**
 * Setup configuration
 */
void setup() {

  // -----------------------------GENERAL setup------------------------------------


  Serial.begin(9600);

  // ------------------------------ADS1115 setup---------------------------------

  Wire.begin();
  ADS.begin();         // Initialisation du module ADS1115
  ADS.setGain(0);      // On prend le gain le plus bas (index 0), pour avoir la plus grande plage de mesure (6.144 volt)
  ADS.setMode(1);      // On indique à l'ADC qu'on fera des mesures à la demande, et non en continu (0 = CONTINUOUS, 1 = SINGLE)
  ADS.setDataRate(7);  // On spécifie la vitesse de mesure de tension qu'on souhaite, allant de 0 à 7 (7 étant le plus rapide, soit 860 échantillons par seconde)
  ADS.readADC(0);      // Et on fait une lecture à vide, pour envoyer tous ces paramètres

  // -----------------------------------------------------------------------------
  
  // ----------------------------- MPU6050 setup ----------------------------------

  // Initialisation de la communication I2C
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); // Registre de configuration du MPU6050
  Wire.write(0); // Réveille le MPU6050
  Wire.endTransmission(true);

  // MPU calibration: set YOUR offsets here.
  mpu.setXAccelOffset(-240);
  mpu.setYAccelOffset(-3189);
  mpu.setZAccelOffset(759);
  mpu.setXGyroOffset(42);
  mpu.setYGyroOffset(-31);
  mpu.setZGyroOffset(-56);

// -----------------------------------------------------------------------------------------------------

// ---------------------------------------DRIVER MINIMOTEURS setup -------------------------------------

    // Configuration des ports en mode "sortie"
    pinMode(motorPinA1, OUTPUT);
    pinMode(motorPinA2, OUTPUT);
    pinMode(enablePinA, OUTPUT);
    pinMode(motorPinB1, OUTPUT);
    pinMode(motorPinB2, OUTPUT);
    pinMode(enablePinB, OUTPUT);


  //----------------------------------------------------------------------------------------------------
  
  //----------------------------------------SERVO setup ---------------------------------------

  servo1.attach(5);  // servo 1
  servo2.attach(9);  // servo 2 

  //------------------------------------------------------------------------------------------
}

/**
 * Main program loop
 */
void loop() {

  // ---------------------------------------ADS1115 loop-----------------------------------------

  lire_can(); //met à jour les valeurs tension_volts_A0/A1

  // ---------------------------------------------------------------------------------------

  // ----------------------------------------RADIO loop-------------------------------------

  lire_radio (); // met à jour barre_hori,barre_vert,plonge

  //----------------------------------------------------------------------------------------

  // ---------------------------------------MPU6050 loop--------------------------------------------

  lire_mpu (); //met à jour les valeurs x,y,z

  // --------------------------------------------------------------------------------------------

  // -------------------------------------DRVER MINIMOTEURS loop---------------------------------

  //les moteurs sont entièrement commandées par les variables ci-dessous
  if (plonge <-50){plonge = -255;}
  else if (plonge >50){plonge = 255;}
  else { equilibre();}
  stateA = -1*plonge;
  stateB = plonge;

  Serial.println(plonge);

  //activation_moteur
  moteurA (stateA);
  moteurB (stateB);
  

  // ------------------------------------------------------------------------------------------

  // -----------------------------------------SERVO loop--------------------------------------

  if(barre_hori <80. || barre_hori >100.){
  servo1.write(barre_hori);}
  else if (servo1.read() <85. || servo1.read() > 95.){servo1.write(90.);}

  if (barre_vert<80. || barre_vert >100.){servo2.write(barre_vert);}
  else if (servo2.read()<85. || servo2.read() > 95.) {servo2.write(90.);}
  

  // ----------------------------------------------------------------------------------------
  delay(100);
}

void lire_radio (){
  barre_hori = map(pulseIn (pin_barre_hori, HIGH), res_min, res_max, 45 , 135);
  barre_vert = map(pulseIn (pin_barre_vert, HIGH), res_min, res_max, 45 , 135);
  plonge = map(pulseIn (pin_plonge, HIGH), res_min, res_max, -1*working_range , working_range);
  if(barre_hori >255){barre_hori = 255;}
  if(barre_vert >255){barre_vert = 255;}
  if(plonge >255){plonge = 255;}
  if(barre_hori <-255){barre_hori = -255;}
  if(barre_vert <-255){barre_vert = -255;}
  if(plonge <-255){plonge = -255;}

  Serial.print("barre_hori=");
  Serial.println(barre_hori);
  Serial.print("barre_vert=");
  Serial.println(barre_vert);

}

void equilibre(){
  if (x <226. && x > 224. && y <226. && y > 224. && z <226. && z > 224.){
    return;
  }
  while (y > 30. && y < 180.){
    Serial.println("j'équilibre");
    moteurA(-255);
    lire_mpu();
    lire_can();
    lire_radio();
    if(barre_hori <80. || barre_hori >100.){
      servo1.write(barre_hori);}
    else if (servo1.read() <88. || servo1.read() > 92.){servo1.write(90.);}

    if (barre_vert<80. || barre_vert >100.){servo2.write(barre_vert);}
    else if (servo2.read()<88. || servo2.read() > 92.) {servo2.write(90.);}
  

  }
  while (y < 330. && y > 180.){
    moteurA(255);
    Serial.println("j'équilibre");
    lire_mpu();
    lire_can();
    lire_radio();
    if(barre_hori <80. || barre_hori >100.){
      servo1.write(barre_hori);}
    else if (servo1.read() <88. || servo1.read() > 92.){servo1.write(90.);}

    if (barre_vert<80. || barre_vert >100.){servo2.write(barre_vert);}
    else if (servo2.read()<88. || servo2.read() > 92.) {servo2.write(90.);}
  
  }
}

void lire_can (){
  // Demande de mesure de tensions à l'ADC (résultats entre -32768 et +32767)
  tension_A0 = ADS.readADC(0);           // Mesure de tension de la broche A0, par rapport à la masse
  tension_volts_A0 = ADS.toVoltage(tension_A0);
  tension_A1 = ADS.readADC(1);           // Mesure de tension de la broche A1, par rapport à la masse
  tension_volts_A1 = ADS.toVoltage(tension_A1);

  // Nota1 : ces valeurs seront en fait comprises entre 0 et +32767, car aucune tension négative ne peut être appliquée sur ces broches

  Serial.print("la tension en A0 est ");
  Serial.print(tension_volts_A0);
  Serial.print(",");
  Serial.print("la tension en A1 est ");
  Serial.println(tension_volts_A1);

}

void lire_mpu (){

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

  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(",");
  Serial.println(z);

}

void moteurA (int stateA){
  if(tension_volts_A0< 0.5){
    analogWrite(enablePinA,0);
    return;
  }
  // Sens du mouvement
    //
    if (stateA > 50 && tension_volts_A0 > 2.5) // avant
    {
      digitalWrite(motorPinA1, HIGH); 
      digitalWrite(motorPinA2, LOW);
    }
    else if (stateA < 50 && tension_volts_A0 < 4.) // arrière
    {
      digitalWrite(motorPinA1, LOW); 
      digitalWrite(motorPinA2, HIGH);
    }
    else // Stop (freinage)
    {
      digitalWrite(motorPinA1, HIGH); 
      digitalWrite(motorPinA2, HIGH);
    }

      //
      // Vitesse du mouvement
      //
    analogWrite(enablePinA, abs(stateA));
}

void moteurB (int stateB){
  if(tension_volts_A1< 0.5){
    analogWrite(enablePinB,0);
    return;
  }
  //
  // Sens du mouvement
  //
  if (stateB > 50 && tension_volts_A1 <4.) // avant
  {
    digitalWrite(motorPinB1, HIGH); 
    digitalWrite(motorPinB2, LOW);
  }
  else if (stateB < 50 && tension_volts_A1 > 2.5) // arrière
  {
    digitalWrite(motorPinB1, LOW); 
    digitalWrite(motorPinB2, HIGH);
  }
  else // Stop (freinage)
  {
    digitalWrite(motorPinB1, HIGH); 
    digitalWrite(motorPinB2, HIGH);
  }

    //
    // Vitesse du mouvement
    //
  analogWrite(enablePinB, abs(stateB));
}
