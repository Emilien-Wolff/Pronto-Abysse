/*
   ______               _                  _///_ _           _                   _
  /   _  \             (_)                |  ___| |         | |                 (_)
  |  [_|  |__  ___  ___ _  ___  _ __      | |__ | | ___  ___| |_ _ __ ___  _ __  _  ___  _   _  ___
  |   ___/ _ \| __|| __| |/ _ \| '_ \_____|  __|| |/ _ \/  _|  _| '__/   \| '_ \| |/   \| | | |/ _ \
  |  |  | ( ) |__ ||__ | | ( ) | | | |____| |__ | |  __/| (_| |_| | | (_) | | | | | (_) | |_| |  __/
  \__|   \__,_|___||___|_|\___/|_| [_|    \____/|_|\___|\____\__\_|  \___/|_| |_|_|\__  |\__,_|\___|
                                                                                      | |
                                                                                      \_|
  Fichier :       prgTestESC-1-arduino.ino
  
  Description :   Programme permettant de piloter un moteur brushless via un ESC
                  depuis un Arduino Nano, monté sur breadboard
                  
  Auteur :        Jérôme TOMSKI (https://passionelectronique.fr/)
  Créé le :       03.02.2022

*/

//*************************//
// Bibliothèques utilisées //
//*************************//
#include <Servo.h>

//**************************************************//
// Définition des entrées/sorties de l'Arduino Nano //
//**************************************************//
#define pinLecturePotentiometre   A0      // [Entrée] Pour mesure de la tension du point milieu du potentiomètre de commande
#define pinPilotageESC             9      // [Sortie] On associe la ligne de pilotage de l'ESC à la broche D9 de l'arduino nano
                                          //          (on aurait très bien pu utiliser une autre pin, entre D2 et D13 compris, avec cette librairie)

//***************************************************//
// Définition de paramètres généraux de ce programme //
//***************************************************//
#define dureeMinimaleImpulsionCommandeESC     1000        // La durée minimale pour une impulsion est de 1000 µs, soit 1 ms (comme pour un servomoteur, en fait)
#define dureeMaximaleImpulsionCommandeESC     2000        // La durée maximale pour une impulsion est de 2000 µs, soit 2 ms (comme pour un servomoteur, idem donc)

//************************//
// Variables du programme //
//************************//
//int valPinLecturePotentiometre;           // Variable 16 bits où sera stockée la lecture 10 bits (0..1023) de l'ADC, du potentiomètre
int dureeImpulsionCommandeESC;            // Valeur exprimée en microseconde, qui contiendra la durée de l'impulsion à envoyer à l'ESC

// Instanciation de la librairie "servo", pour piloter le module ESC
Servo moduleESC;

int i = 50;

// ========================
// Initialisation programme
// ========================
void setup() {

  // Configuration des pins de sortie de l'Arduino Nano
  pinMode(pinPilotageESC, OUTPUT);

  // Associe l'objet ESC à la broche de commande de l'ESC, avec précision des durées d'impulsion mini/maxi
  moduleESC.attach(pinPilotageESC, dureeMinimaleImpulsionCommandeESC, dureeMaximaleImpulsionCommandeESC);

  moduleESC.writeMicroseconds(1000);
  delay(1000);

  Serial.begin(9600);

}


// =================
// Boucle principale
// =================
void loop() {
 /* int valPinLecturePotentiometre;
  // Lecture de la tension retournée par le potentiomètre (valeur comprise entre 0 et 1023)
  valPinLecturePotentiometre = analogRead(pinLecturePotentiometre);

  // Conversion de cette valeur 0..1023, en une valeur comprise entre duréeMin et duréeMax de l'impulsion à produire
  dureeImpulsionCommandeESC = map(valPinLecturePotentiometre, 0, 1023, dureeMinimaleImpulsionCommandeESC, dureeMaximaleImpulsionCommandeESC);

  // Envoi du signal de commande à l'ESC
  moduleESC.writeMicroseconds(dureeImpulsionCommandeESC);
  Serial.println(dureeImpulsionCommandeESC);*/
  dureeImpulsionCommandeESC = map (i,0,100, dureeMinimaleImpulsionCommandeESC, dureeMaximaleImpulsionCommandeESC);
  moduleESC.writeMicroseconds(dureeImpulsionCommandeESC);
  i+=10;
  if (i >= 100){
    i = 0;
  }  
  // … et rebouclage à l'infini !
  delay(1000);
  
}