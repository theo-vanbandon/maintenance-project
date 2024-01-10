/**
 * Fichier Implémentation de la classe
 * Nom du fichier : ClasseNVS.cpp
 * Nom de la classe : ClasseNVS
 * Description : Fichier d'implémentation de la classe ClasseNVS
 * Auteur : Théo Vanbandon
 * Date : 27/02/2023
**/

#include "ClasseNVS.hpp"
#include <iostream>
#include <string>

Preferences prefs;

//Constructeur
ClasseNVS::ClasseNVS() {
    Serial.begin(115200);
    Serial.println();
}

//Destructeur
ClasseNVS::~ClasseNVS() {
}

//Créer un namespace
unsigned char ClasseNVS::CreationNamespace(const char *nomNamespace) {
    unsigned char codErr = 0;
    //Open Preferences with my-app namespace. Each application module, library, etc
    //has to use a namespace name to prevent key name collisions. We will open storage in
    //RW-mode (second parameter has to be false).
    //Note: Namespace name is limited to 15 chars.
    prefs.begin(nomNamespace, false);
    codErr = 1;
    return codErr;
}

//Création de variable String
unsigned char ClasseNVS::AddValueStr(int index, const char * valeurVariable) {
    unsigned char codErr = 0;

    switch (index) {
    case 0:
        //Get the counter value, if the key does not exist, return a default value of 0
        //Note: Key name is limited to 15 chars.
        prefs.getString("TypeObjet", "00");
        //Choix du type d'objet
        TypeObjet = valeurVariable;
        //Print the counter to Serial Monitor
        Serial.println("Valeur de TypeObjet:\t" + TypeObjet);
        //Store the counter to the Preferences
        prefs.putString("TypeObjet", TypeObjet);
        codErr = 1;
        break;

    case 1:
        //Get the counter value, if the key does not exist, return a default value of 0
        //Note: Key name is limited to 15 chars.
        prefs.getString("SSID", "00");
        //Valeur du SSID
        SSID = valeurVariable;
        //Print the counter to Serial Monitor
        Serial.println("Valeur de SSID:\t\t" + SSID);
        //Store the counter to the Preferences
        prefs.putString("SSID", SSID);
        codErr = 1;
        break;

    case 2:
        //Get the counter value, if the key does not exist, return a default value of 0
        //Note: Key name is limited to 15 chars.
        prefs.getString("Password", "00");
        //Valeur du mot de passe
        Password = valeurVariable;
        //Print the counter to Serial Monitor
        Serial.println("Valeur de Password:\t" + Password);
        //Store the counter to the Preferences
        prefs.putString("Password", Password);
        codErr = 1;
        break;

    case 3:
        //Get the counter value, if the key does not exist, return a default value of 0
        //Note: Key name is limited to 15 chars.
        prefs.getString("UUID_Moteur", "00");
        //Identifiant du moteur
        UUID_Moteur = valeurVariable;
        //Print the counter to Serial Monitor
        Serial.println("Valeur de UUID_Moteur:\t" + UUID_Moteur);
        //Store the counter to the Preferences
        prefs.putString("UUID_Moteur", UUID_Moteur);
        codErr = 1;
        break;

    case 5:
        //Get the counter value, if the key does not exist, return a default value of 0
        //Note: Key name is limited to 15 chars.
        prefs.getString("UUID_Machine", "00");
        //Identifiant du moteur
        UUID_Machine = valeurVariable;
        //Print the counter to Serial Monitor
        Serial.println("Valeur de UUID_Machine:\t" + UUID_Machine);
        //Store the counter to the Preferences
        prefs.putString("UUID_Machine", UUID_Machine);
        codErr = 1;
        break;

    default:
        codErr = 2;
        break;
    }
    return codErr;
}


//Création de variable Int
unsigned char ClasseNVS::AddValueInt(int index, unsigned int valeurVariable) {
    unsigned char codErr;

    if (index == 4) {
        //Get the counter value, if the key does not exist, return a default value of 0
        //Note: Key name is limited to 15 chars.
        Frequence = prefs.getUInt("Frequence", valeurVariable);
        //Incrémentation
        Frequence++;
        //Print the counter to Serial Monitor
        Serial.printf("Valeur de la fréquence:\t%u\n", Frequence);
        //Store the counter to the Preferences
        prefs.putUInt("Frequence", Frequence);
        codErr = 1;
    } else {
        codErr = 2;
    }
    
    return codErr;
}



//Suppression de toutes les préférences sous le namespace ouvert
unsigned char ClasseNVS::RemoveAllPref() {
    unsigned char codErr = 0;

    //Remove all preferences under the opened namespace
    prefs.clear();
    codErr = 1;

    return codErr;
}

//Suppression d'une seule préférence en particulier
unsigned char ClasseNVS::RemovePref(const char * nomPref) {
    unsigned char codErr = 0;

    //Or remove the counter key only
    prefs.remove(nomPref);
    codErr = 1;
    
    return codErr;
}

//Close Preference
unsigned char ClasseNVS::ClosePref() {
    unsigned codErr = 0;

    //Close the Preferences
    prefs.end();

    codErr = 1;
    return codErr;
}