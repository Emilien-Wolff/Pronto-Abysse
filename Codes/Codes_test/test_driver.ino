// Pont en H L298N
#include <Wire.h> 

//Ports de commande du moteur B
int motorPin1 = 8;
int motorPin2 = 7;
int enablePin = 5;
 
// Vitesse du moteur
int state = 0;

void setup() {
    // Configuration des ports en mode "sortie"
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(enablePin, OUTPUT);

    // Initialisation du port série
    Serial.begin(9600);

    analogReadResolution(12);
}


 
void loop() {

    // Lecture de l'entier passé au port série
    state = 250;

    //
    // Sens du mouvement
    //
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
    delay(1000);
}