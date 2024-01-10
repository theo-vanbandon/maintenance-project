// #include <Arduino.h>
// #include <Preferences.h>

// Preferences prefs;

// void setup(void) {
//   Serial.begin(115200);
//   Serial.println();

//   //Open Preferences with my-app namespace. Each application module, library, etc
//   //has to use a namespace name to prevent key name collisions. We will open storage in
//   //RW-mode (second parameter has to be false).
//   //Note: Namespace name is limited to 15 chars.
//   prefs.begin("my-app", false);

//   //Remove all preferences under the opened namespace
//   //prefs.clear();

//   // Or remove the counter key only
//   // prefs.remove("counter");

//   //Get the counter value, if the key does not exist, return a default value of 0
//   //Note: Key name is limited to 15 chars.
//   String TypeObjet = prefs.getString("TypeObjet", "00");
//   //Choix du type d'objet
//   TypeObjet = "Vibration";
//   //Print the counter to Serial Monitor
//   Serial.println("Valeur de TypeObjet:\t" + TypeObjet);
//   //Store the counter to the Preferences
//   prefs.putString("TypeObjet", TypeObjet);

//   //Get the counter value, if the key does not exist, return a default value of 0
//   //Note: Key name is limited to 15 chars.
//   String SSID = prefs.getString("SSID", "00");
//   //Valeur du SSID
//   SSID = "tpsnir";
//   //Print the counter to Serial Monitor
//   Serial.println("Valeur de SSID:\t\t" + SSID);
//   //Store the counter to the Preferences
//   prefs.putString("SSID", SSID);

//   //Get the counter value, if the key does not exist, return a default value of 0
//   //Note: Key name is limited to 15 chars.
//   String Password = prefs.getString("Password", "00");
//   //Valeur du mot de passe
//   Password = "t2p0s2n0";
//   //Print the counter to Serial Monitor
//   Serial.println("Valeur de Password:\t" + Password);
//   //Store the counter to the Preferences
//   prefs.putString("Password", Password);

//   //Get the counter value, if the key does not exist, return a default value of 0
//   //Note: Key name is limited to 15 chars.
//   String UUID_Moteur = prefs.getString("UUID_Moteur", "00");
//   //Identifiant du moteur
//   UUID_Moteur = "Je sais pas 1";
//   //Print the counter to Serial Monitor
//   Serial.println("Valeur de UUID_Moteur:\t" + UUID_Moteur);
//   //Store the counter to the Preferences
//   prefs.putString("UUID_Moteur", UUID_Moteur);

//   //Get the counter value, if the key does not exist, return a default value of 0
//   //Note: Key name is limited to 15 chars.
//   unsigned int Frequence = prefs.getUInt("Frequence", 0);
//   //Choix de la fréquence
//   Frequence = 20;
//   //Print the counter to Serial Monitor
//   Serial.printf("Valeur de la fréquence:\t%u\n", Frequence);
//   //Store the counter to the Preferences
//   prefs.putUInt("Frequence", Frequence);

//   //Get the counter value, if the key does not exist, return a default value of 0
//   //Note: Key name is limited to 15 chars.
//   String UUID_Machine = prefs.getString("UUID_Machine", "00");
//   //Identifiant du moteur
//   UUID_Machine = "Je sais pas 2";
//   //Print the counter to Serial Monitor
//   Serial.println("Valeur de UUID_Machine:\t" + UUID_Machine);
//   //Store the counter to the Preferences
//   prefs.putString("UUID_Machine", UUID_Machine);

//   //Close the Preferences
//   prefs.end();
// }

// void loop(void) {
//   //Wait 10 seconds
//   Serial.println("Restarting in 10 seconds...");
//   delay(10000);

//   //Restart ESP
//   ESP.restart();
// }





#include <Arduino.h>
//#include <Preferences.h>
#include "ClasseNVS.hpp"

void setup(void) {
  ClasseNVS objNVS;

  objNVS.CreationNamespace("my-app");

  objNVS.AddValueStr(0, "Vibration");            //TypeObjet
  objNVS.AddValueStr(1, "tpsnir");               //SSID
  objNVS.AddValueStr(2, "t2p0s2n0");             //Password
  objNVS.AddValueStr(3, "IdentifiantMoteur");    //UUID_Moteur
  objNVS.AddValueInt(4, 20);                     //Frequence
  objNVS.AddValueStr(5, "IdentifiantMachine");   //UUID_Machine

  objNVS.ClosePref();
}

void loop(void) {
  //Wait 10 seconds
  Serial.println("Restarting in 10 seconds...");
  delay(10000);

  //Restart ESP
  ESP.restart();
}