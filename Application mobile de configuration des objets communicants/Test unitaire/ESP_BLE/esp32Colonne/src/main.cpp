// #include <Arduino.h>
// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEServer.h>

// //Définition des UUID des services et caractéristiques
// //Pour générer des UUID cf https://www.uuidgenerator.net/
// #define SERVICE_CONFIG_ESP "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
// #define CARACT_TYPE_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
// #define CARACT_SSID_UUID "27353122-98af-11ed-a8fc-0242ac120002"
// #define CARACT_PASS_UUID "83b26702-a2fb-11ed-a8fc-0242ac120002"
// #define CARACT_FREQUENCE_UUID "2016d454-a2ff-11ed-a8fc-0242ac120452"
// #define CARACT_MOTEUR_UUID "8d5dacb2-a2fb-11ed-a8fc-0242ac120142"
// #define CARACT_MACHINE_UUID "8ff0c73e-a2fb-11ed-a8fc-0242ac120892"

// //Déclaration des pointeurs sur les objets BLE
// BLEServer *ptrServSNIR = NULL;
// BLEService *ptrServiceConfigESP = NULL;

// BLECharacteristic *ptrCaractType = NULL;
// BLEDescriptor *ptrDescrType = NULL;

// BLECharacteristic *ptrCaractSSID = NULL;
// BLEDescriptor *ptrDescrSSID = NULL;

// BLECharacteristic *ptrCaractPass = NULL;
// BLEDescriptor *ptrDescrPass = NULL;

// BLECharacteristic *ptrCaractFrequence = NULL;
// BLEDescriptor *ptrDescrFrequence = NULL;

// BLECharacteristic *ptrCaractMoteur = NULL;
// BLEDescriptor *ptrDescrMoteur = NULL;

// BLECharacteristic *ptrCaractMachine = NULL;
// BLEDescriptor *ptrDescrMachine = NULL;

// //flag pour savoir si un client est connecté
// bool deviceBLEConnected = false;

// // Déclaration des Callbacks de connexion au serveur BLE
// class ServSNIRCallbacks : public BLEServerCallbacks {
//   void onConnect(BLEServer *pServer) {
//     Serial.println("Client connecté");
//     deviceBLEConnected = true;
//   };
//   void onDisconnect(BLEServer *pServer) {
//     Serial.println("Client déconnecté");
//     deviceBLEConnected = false;
//   }
// };

// // Déclaration des Callbacks de lecture / écriture d'une caractéristique
// class CaractCallbacks : public BLECharacteristicCallbacks {
//   void onRead(BLECharacteristic *pCharacteristic) {
//     std::string carcatUUID = pCharacteristic->getUUID().toString();
//     std::string value = pCharacteristic->getValue();
//     Serial.print("Lecture de la caractéristique : ");
//     Serial.print((String)carcatUUID.c_str() + " = ");
//     Serial.println((String)value.c_str());
//   }

//   void onWrite(BLECharacteristic *pCharacteristic) {
//     std::string carcatUUID = pCharacteristic->getUUID().toString();
//     std::string value = pCharacteristic->getValue();
//     Serial.print("Modidification de la caractéristique : ");
//     Serial.print((String)carcatUUID.c_str() + " = ");
//     Serial.println((String)value.c_str());
//   }
// };

// void setup() {
//   Serial.begin(115200);
//   Serial.println();
//   Serial.println("Configuration du serveur BLE...");

//   // 1. Initialiser le périphérique BLE et lui donner un nom
//   BLEDevice::init("Config ESP32 BLE");
//   // 2. Créer un serveur BLE
//   ptrServSNIR = BLEDevice::createServer();
//   // 2b. Associer les fonctions de callback onConnect et onDisconnect
//   ptrServSNIR->setCallbacks(new ServSNIRCallbacks());
//   // 3. Créer un service BLE
//   ptrServiceConfigESP = ptrServSNIR->createService(SERVICE_CONFIG_ESP);
//   // 4. Créer une caractéristique BLE associée au service
//   // accessible en lecture et écriture
//   ptrCaractType = ptrServiceConfigESP->createCharacteristic(CARACT_TYPE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
//   ptrCaractSSID = ptrServiceConfigESP->createCharacteristic(CARACT_SSID_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
//   ptrCaractPass = ptrServiceConfigESP->createCharacteristic(CARACT_PASS_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
//   ptrCaractFrequence = ptrServiceConfigESP->createCharacteristic(CARACT_FREQUENCE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
//   ptrCaractMoteur = ptrServiceConfigESP->createCharacteristic(CARACT_MOTEUR_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
//   ptrCaractMachine = ptrServiceConfigESP->createCharacteristic(CARACT_MACHINE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
//   // 4b. Associer les fonctions de callback onRead et onWrite
//   ptrCaractType->setCallbacks(new CaractCallbacks());
//   ptrCaractSSID->setCallbacks(new CaractCallbacks());
//   // ptrCaractPass->setCallbacks(new CaractCallbacks());
//   ptrCaractFrequence->setCallbacks(new CaractCallbacks());
//   ptrCaractMoteur->setCallbacks(new CaractCallbacks());
//   ptrCaractMachine->setCallbacks(new CaractCallbacks());
//   // 5. Ajouter une description textuelle à la caractéristique
//   // Le Descriptor est ici de type "Client Characteristic Description (CCCD)",
//   // ce qui correspond à l'UUID 0x2902.
//   ptrDescrType = new BLEDescriptor(BLEUUID((uint16_t)0x2902));
//   ptrDescrType->setValue("Type");
//   ptrCaractType->addDescriptor(ptrDescrType);

//   ptrDescrSSID = new BLEDescriptor(BLEUUID((uint16_t)0x2902));
//   ptrDescrSSID->setValue("SSID");
//   ptrCaractSSID->addDescriptor(ptrDescrSSID);

//   ptrDescrPass = new BLEDescriptor(BLEUUID((uint16_t)0x2902));
//   ptrDescrPass->setValue("Password");
//   ptrCaractPass->addDescriptor(ptrDescrPass);

//   ptrDescrFrequence = new BLEDescriptor(BLEUUID((uint16_t)0x2902));
//   ptrDescrFrequence->setValue("Frequence");
//   ptrCaractFrequence->addDescriptor(ptrDescrFrequence);

//   ptrDescrMoteur = new BLEDescriptor(BLEUUID((uint16_t)0x2902));
//   ptrDescrMoteur->setValue("ID Moteur");
//   ptrCaractMoteur->addDescriptor(ptrDescrMoteur);

//   ptrDescrMachine = new BLEDescriptor(BLEUUID((uint16_t)0x2902));
//   ptrDescrMachine->setValue("ID Machine");
//   ptrCaractMachine->addDescriptor(ptrDescrMachine);
//   // 6. Initialiser la valeur de la caractéristique (512 octets max)
//   // types de valeur : string, uint8_t [] (évitez les autres types)
//   ptrCaractType->setValue("Vibration");
//   ptrCaractSSID->setValue("SSID");
//   ptrCaractPass->setValue("Pass");
//   ptrCaractFrequence->setValue("Frequence");
//   ptrCaractMoteur->setValue("ID Moteur");
//   ptrCaractMachine->setValue("ID Machine");
//   // 7. Démarrer le service
//   ptrServiceConfigESP->start();
//   // 8. Créer un objet de gestion de l'advertising
//   // diffusion du profil BLE pour que les clients puissent découvrir le serveur
//   BLEAdvertising *ptrAdvertising = BLEDevice::getAdvertising();
//   // 9. Associer le service à l'advertising
//   ptrAdvertising->addServiceUUID(SERVICE_CONFIG_ESP);
//   ptrAdvertising->setScanResponse(true);
//   ptrAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
//   ptrAdvertising->setMinPreferred(0x12);
//   // 10. Démarrer l’advertising
//   BLEDevice::startAdvertising();
//   Serial.println("Serveur BLE prêt !");
// }

// void loop() {
//   //relancer l'advertising régulièrement s'il n'y a pas de client connecté
//   if (deviceBLEConnected == false)
//     BLEDevice::startAdvertising();

//   delay(500);
// }


#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

//Définition des UUID des services et caractéristiques
//Pour générer des UUID cf https://www.uuidgenerator.net/
#define SERVICE_CONFIG_ESP "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
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
  }
};

void setup() {
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