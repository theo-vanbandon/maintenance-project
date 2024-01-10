#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "ClasseNVS.hpp"
#include <ArduinoJson-v6.20.1.h>

//Définition des UUID des services et caractéristiques
//Pour générer des UUID cf https://www.uuidgenerator.net/
#define SERVICE_CONFIG_ESP "4fafc201-1fb5-278d-8fcc-c5c9c331914b"
#define CARACT_CONFIG_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

//Déclaration des pointeurs sur les objets BLE
BLEServer *ptrServSNIR = NULL;
BLEService *ptrServiceConfigESP = NULL;

BLECharacteristic *ptrCaractConfig = NULL;
BLEDescriptor *ptrDescrConfig = NULL;

//flag pour savoir si un client est connecté
bool deviceBLEConnected = false;

// Déclaration des Callbacks de connexion au serveur BLE
class ServSNIRCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    Serial.println("Client connecté");
    deviceBLEConnected = true;
  };
  void onDisconnect(BLEServer *pServer) {
    Serial.println("Client déconnecté");
    deviceBLEConnected = false;
  }
};

// Déclaration des Callbacks de lecture / écriture d'une caractéristique
class CaractCallbacks : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pCharacteristic) {
    std::string carcatUUID = pCharacteristic->getUUID().toString();
    std::string value = pCharacteristic->getValue();
    Serial.print("Lecture de la caractéristique : ");
    Serial.print((String)carcatUUID.c_str() + " = ");
    Serial.println((String)value.c_str());
  }

  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string carcatUUID = pCharacteristic->getUUID().toString();
    std::string value = pCharacteristic->getValue();
    Serial.print("Modification de la caractéristique : ");
    Serial.print((String)carcatUUID.c_str() + " = ");
    Serial.println((String)value.c_str());

    //Récupération des valeurs du JSON
    String json = (String)value.c_str();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);
    String typeStr = doc["type"];

    const char * type = doc["type"];
    const char * SSID = doc["SSIDPointAcces"];
    const char * Pass = doc["Password"];

    int frequence;
    const char * UUIDMoteur;
    const char * UUIDMachine;

    if (typeStr == "vib") {
      frequence = doc["Frequence"];
      UUIDMoteur = doc["IDMoteur"];

    }
    if (typeStr == "col") {
      UUIDMachine = doc["IDMachine"];
    }


    //NVS
    ClasseNVS objNVS;
    objNVS.CreationNamespace("my-app");

    objNVS.AddValueStr(0, type);        //TypeObjet
    objNVS.AddValueStr(1, SSID);         //SSID
    objNVS.AddValueStr(2, Pass);         //Password
    if (typeStr == "vib") {
      objNVS.AddValueStr(3, UUIDMoteur);  //UUID_Moteur
      objNVS.AddValueInt(4, frequence);   //Frequence
      objNVS.AddValueStr(5, "0");         //UUID_Machine
    }
    if (typeStr == "col") {
      objNVS.AddValueStr(3, "0");         //UUID_Moteur
      objNVS.AddValueInt(4, 0);           //Frequence
      objNVS.AddValueStr(5, UUIDMachine); //UUID_Machine
    }
    
    objNVS.ClosePref();
  }
};

void setup() {
  //BLE
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuration du serveur BLE...");

  // 1. Initialiser le périphérique BLE et lui donner un nom
  BLEDevice::init("Config ESP32 BLE");
  // 2. Créer un serveur BLE
  ptrServSNIR = BLEDevice::createServer();
  // 2b. Associer les fonctions de callback onConnect et onDisconnect
  ptrServSNIR->setCallbacks(new ServSNIRCallbacks());
  // 3. Créer un service BLE
  ptrServiceConfigESP = ptrServSNIR->createService(SERVICE_CONFIG_ESP);
  // 4. Créer une caractéristique BLE associée au service
  // accessible en lecture et écriture
  ptrCaractConfig = ptrServiceConfigESP->createCharacteristic(CARACT_CONFIG_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  // 4b. Associer les fonctions de callback onRead et onWrite
  ptrCaractConfig->setCallbacks(new CaractCallbacks());
  // 5. Ajouter une description textuelle à la caractéristique
  // Le Descriptor est ici de type "Client Characteristic Description (CCCD)",
  // ce qui correspond à l'UUID 0x2902.
  ptrDescrConfig = new BLEDescriptor(BLEUUID((uint16_t)0x2902));
  ptrDescrConfig->setValue("Test");
  ptrCaractConfig->addDescriptor(ptrDescrConfig);
  // 6. Initialiser la valeur de la caractéristique (512 octets max)
  // types de valeur : string, uint8_t [] (évitez les autres types)
  ptrCaractConfig->setValue("Vibration");
  // 7. Démarrer le service
  ptrServiceConfigESP->start();
  // 8. Créer un objet de gestion de l'advertising
  // diffusion du profil BLE pour que les clients puissent découvrir le serveur
  BLEAdvertising *ptrAdvertising = BLEDevice::getAdvertising();
  // 9. Associer le service à l'advertising
  ptrAdvertising->addServiceUUID(SERVICE_CONFIG_ESP);
  ptrAdvertising->setScanResponse(true);
  ptrAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  ptrAdvertising->setMinPreferred(0x12);
  // 10. Démarrer l’advertising
  BLEDevice::startAdvertising();
  Serial.println("Serveur BLE prêt !");
}

void loop() {
  //relancer l'advertising régulièrement s'il n'y a pas de client connecté
  if (deviceBLEConnected == false)
    BLEDevice::startAdvertising();

  delay(500);
}