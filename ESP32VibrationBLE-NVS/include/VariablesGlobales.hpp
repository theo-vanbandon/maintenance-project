/**
 * Fichier Interface de l'unite
 * Nom du fichier : VariablesGlobales.hpp
 * Nom de l'unite : VariablesGlobales
 * Description : Fichier d'interface des variables globales 
 * Auteur : WAELES Swann
 * Date : 27/01/2023
**/
#include <SparkFun_ADXL345.h>
#include <Arduino.h>
#include <arduinoFFT.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <BLEUtils.h>   //Bibliothèque pour le BLE
#include <BLEDevice.h>  //Bibliothèque pour le BLE
#include <BLEServer.h>  //Bibliothèque pour le BLE
#include "ClasseNVS.hpp"    //Bibliothèque pour le NVS
#pragma once

// Broches de l'ESP32
#define CS_PIN 15
#define MISO_PIN 12 // adxl345 SD0
#define MOSI_PIN 13 // adxl345 SDA
#define SCK_PIN 14

//Déclarations pour MQTT
#define mqtt_server_ip "172.17.129.128" //broker mosquitto
#define mqtt_server_port 1883        //broker mosquitto
#define mqtt_user "guest"            //s'il a été configuré sur Mosquitto
#define mqtt_password "guest"        //idem

// Déclarations pour le réseau WiFi
extern const char *ssid;         // SSID du réseau WiFi
extern const char *password; // passphrase du réseau WiFi

extern const char *monPublishTopic;
extern unsigned long lastMsg;


#define NB_ECHANTILLONS 64 //doit être un multiple de 2 pour le calcul de la FFT
#define FREQU_ECHANTILLON 2000 //fréquence d'échantillonnage 2000Hz
#define PERIODE_ECHANTILLON 1000000 / FREQU_ECHANTILLON // Lecture à 2000Hz -> 1/2000 = 500μs

extern double dataX[NB_ECHANTILLONS];
extern double imagX[NB_ECHANTILLONS];
extern double dataY[NB_ECHANTILLONS];
extern double imagY[NB_ECHANTILLONS];
extern double dataZ[NB_ECHANTILLONS];
extern double imagZ[NB_ECHANTILLONS];

extern unsigned long previousMicros;
extern unsigned long currentMicros;
extern int numMesures;

// Création d'un objet de la lib ADXL345 pour accéléromètre connecté en SPI
extern ADXL345 adxl;

//variables globales
extern arduinoFFT FFT;
extern int x, y, z; //valeurs d'accélération sur les 3 axes

//tableaux pour la raie principale de chaque axe
extern unsigned int indiceRaieX;
extern float frequenceRaieX;
extern float accelRaieX;

//Valeurs recupérées en NVS
extern String SsidNVS;
extern String PassNVS;
extern unsigned int FrequenceNVS;
extern String GUID_MoteurNVS;