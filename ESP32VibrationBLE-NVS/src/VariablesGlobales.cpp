/**
 * Fichier Implémentation de l'unite
 * Nom du fichier : 
 * Nom de l'unite : 
 * Description : 
 * Auteur : 
 * Date : 
**/

#include "VariablesGlobales.hpp"

ADXL345 adxl = ADXL345(CS_PIN);
const char *ssid = "tpsnir"; 
const char *password = "t2p0s2n0";
const char *monPublishTopic = "SpectreVib";

unsigned long lastMsg = 0;

unsigned long previousMicros = 0;
unsigned long currentMicros = 0;
int numMesures = 0;

arduinoFFT FFT = arduinoFFT(); 

double dataX[NB_ECHANTILLONS] = {0};
double imagX[NB_ECHANTILLONS] = {0};
double dataY[NB_ECHANTILLONS] = {0};
double imagY[NB_ECHANTILLONS] = {0};
double dataZ[NB_ECHANTILLONS] = {0};
double imagZ[NB_ECHANTILLONS] = {0};

int x = 0;
int y = 0;
int z = 0;

unsigned int indiceRaieX = 0;
float frequenceRaieX = 0;
float accelRaieX = 0;

unsigned int frequMesure = 30000;

//Valeurs recupérées en NVS
String SsidNVS;
String PassNVS;
unsigned int FrequenceNVS;
String GUID_MoteurNVS;