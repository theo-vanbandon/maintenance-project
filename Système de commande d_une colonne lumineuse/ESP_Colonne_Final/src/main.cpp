#include <Arduino.h>
#include <WiFi.h>           //Bibliothèque WiFi pour ESP32
#include <PubSubClient.h>   //Bibliothèque pour MQTT
#include <ArduinoJson.h>    //Bibliothèque pour le JSON
#include <BLEDevice.h>      //Bibliothèque pour le BLE
#include <BLEUtils.h>       //Bibliothèque pour le BLE
#include <BLEServer.h>      //Bibliothèque pour le BLE
#include "ClasseNVS.hpp"    //Bibliothèque pour la NVS

String SsidNVS = "";
String PassNVS = "";
String GUID_MachineNVS = "";

//Déclarations pour le réseau WiFi
const char * ssid = "";     //SSID du réseau WiFi
const char * password = ""; //passphrase du réseau WiFi

//Déclarations pour MQTT
#define mqtt_server_ip "172.17.129.128" //Broker mosquitto
#define mqtt_server_port 1883           //Broker mosquitto
#define mqtt_user "guest"               //S'il a été configuré sur Mosquitto, sinon chaine vide
#define mqtt_password "guest"           //Idem
const char *monSubscribeTopic = "Alerte";

//Broche GPIO32 ESP32
int SIG_PIN = 32;

//Variable concernant l'état de la colonne
int etatColonne = -1;

WiFiClient clientTCP; //Socket TCP
PubSubClient clientMqtt(clientTCP);

//--------------BLE--------------

//Définition des UUID des services et caractéristiques
//Pour générer des UUID cf https://www.uuidgenerator.net/
#define SERVICE_CONFIG_ESP "4fafc201-1fb5-278d-8fcc-c5c9c331914b"
#define CARACT_CONFIG_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

//Déclaration des pointeurs sur les objets BLE
BLEServer *ptrServSNIR = NULL;
BLEService *ptrServiceConfigESP = NULL;

BLECharacteristic *ptrCaractConfig = NULL;
BLEDescriptor *ptrDescrConfig = NULL;

//Flag pour savoir si un client est connecté
bool deviceBLEConnected = false;

//Déclaration des Callbacks de connexion au serveur BLE
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

//Déclaration des Callbacks de lecture / écriture d'une caractéristique
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

    objNVS.AddValueStr(0, type);         //TypeObjet
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

    ESP.restart();  //Après la configuration, on redémarre l'ESP
  }
};

void VerifAvecGUIDMachine(String prmMessage) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, prmMessage);

  Serial.println(GUID_MachineNVS);

  String zone = doc["zone"];
  String guidMachine = doc["guid"];
  Serial.println("Zone: " + zone + " | GUID de la machine: " + guidMachine);

  //Si c'est la bonne colonne on allume en fonction de la zone
  if (guidMachine == GUID_MachineNVS) {
    if (zone == "A" || zone == "B") {
      etatColonne = 0;  //Etat de la colonne = éteint
      Serial.println("Etat de la colonne: 0");
    }
    if (zone == "C" ||zone == "D") {
      etatColonne = 1;  //Etat de la colonne = allumée
      Serial.println("Etat de la colonne: 1");
    }
    Serial.println();
  } else {
    Serial.println("Pas concerné par l'UUID envoyé");
  }
}

/**
 * @brief  Connexion au réseau WiFi (Accès réseau + IP)
 */
void connecter_WiFi() {
  //Connexion au réseau WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.println("Connexion au SSID = " + (String)ssid + " ...");

  //Attendre que la connexion WiFi soit établie
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Nouvelle tentative");
  }

  //Quand on est connecté au réseau WiFi, le serveur DHCP attribue automatiquement la configuration IP
  Serial.println("Connection établie !");
  Serial.println("Adresse MAC ESP = " + WiFi.macAddress());
  Serial.println("Adresse IP ESP = " + WiFi.localIP().toString());
}

/**
 * @brief  Connexion au broker et abonnement
 */
void connecter_broker() {
  //Reconnecter le WiFi si besoin
  if (WiFi.status() != WL_CONNECTED) {
    connecter_WiFi();
  }
  //Boucle jusqu'à obtenir une connexion
  while (clientMqtt.connected() == false) {
    Serial.print("Connexion au serveur MQTT...");
    //Le premier paramètre de la méthode connect() doit être un identifiant unique de l'ESP (ici c'est son adresse MAC)
    bool etatConnexionBroker = false;
    if (mqtt_user == "") {
      etatConnexionBroker = clientMqtt.connect(WiFi.macAddress().c_str());
    }
    else {
      etatConnexionBroker = clientMqtt.connect(WiFi.macAddress().c_str(), mqtt_user, mqtt_password);
    }
    if (etatConnexionBroker == true) {
      Serial.println("OK");
      Serial.println();
      //Abonnement au topic
      clientMqtt.subscribe(monSubscribeTopic);
    }
    else {
      Serial.print("Erreur : ");
      Serial.print(clientMqtt.state());
      Serial.println(" On attend 5 secondes avant de recommencer");
      delay(5000);
    }
  }
}

/**
 * @brief  Déclenche les actions à la réception d'un message MQTT
 */
void onReceiveMsg(char *topic, byte *payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < (int)length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message reçu =>  topic: " + String(topic));
  Serial.print(" | longueur: " + String(length, DEC) + " ");
  Serial.println("message = " + message);

  VerifAvecGUIDMachine(message);
}

void getValueNVS() {
  ClasseNVS objTest;
  SsidNVS = objTest.LireValeurStr(1); //Lecture du SSID dans la NVS
  Serial.println(SsidNVS);

  PassNVS = objTest.LireValeurStr(2); //Lecture du Password dans la NVS
  Serial.println(PassNVS);

  GUID_MachineNVS = objTest.LireValeurStr(5); //Lecture de l'identifiant de la machine dans la NVS
  Serial.println(GUID_MachineNVS);

  //Pour se connecter au WiFi, il faut des variables en const char *
  ssid = &SsidNVS[0];
  Serial.println(ssid);
  password = &PassNVS[0];
  Serial.println(password);
}

void setup() {
  pinMode(SIG_PIN, OUTPUT); //Broche GPIO32 en tant que sortie

  //Récupération des valeurs stockées en NVS
  getValueNVS();

  //--------------BLE--------------
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuration du serveur BLE...");

  //1. Initialiser le périphérique BLE et lui donner un nom
  BLEDevice::init("Config ESP32 BLE");
  //2. Créer un serveur BLE
  ptrServSNIR = BLEDevice::createServer();
  //2b. Associer les fonctions de callback onConnect et onDisconnect
  ptrServSNIR->setCallbacks(new ServSNIRCallbacks());
  //3. Créer un service BLE
  ptrServiceConfigESP = ptrServSNIR->createService(SERVICE_CONFIG_ESP);
  //4. Créer une caractéristique BLE associée au service
  //accessible en lecture et écriture
  ptrCaractConfig = ptrServiceConfigESP->createCharacteristic(CARACT_CONFIG_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  //4b. Associer les fonctions de callback onRead et onWrite
  ptrCaractConfig->setCallbacks(new CaractCallbacks());
  //5. Ajouter une description textuelle à la caractéristique
  //Le Descriptor est ici de type "Client Characteristic Description (CCCD)",
  //ce qui correspond à l'UUID 0x2902.
  ptrDescrConfig = new BLEDescriptor(BLEUUID((uint16_t)0x2902));
  ptrDescrConfig->setValue("Test");
  ptrCaractConfig->addDescriptor(ptrDescrConfig);
  //6. Initialiser la valeur de la caractéristique (512 octets max)
  //types de valeur : string, uint8_t [] (évitez les autres types)
  ptrCaractConfig->setValue("Vibration");
  //7. Démarrer le service
  ptrServiceConfigESP->start();
  //8. Créer un objet de gestion de l'advertising
  //diffusion du profil BLE pour que les clients puissent découvrir le serveur
  BLEAdvertising *ptrAdvertising = BLEDevice::getAdvertising();
  //9. Associer le service à l'advertising
  ptrAdvertising->addServiceUUID(SERVICE_CONFIG_ESP);
  ptrAdvertising->setScanResponse(true);
  ptrAdvertising->setMinPreferred(0x06); //functions that help with iPhone connections issue
  ptrAdvertising->setMinPreferred(0x12);
  //10. Démarrer l’advertising
  BLEDevice::startAdvertising();
  Serial.println("Serveur BLE prêt !");


  //Se connecter au réseau WiFi
  connecter_WiFi();
  //Configurer l'adresse du broker MQTT
  clientMqtt.setServer(mqtt_server_ip, mqtt_server_port);
  //Donner le nom de la fonction qui sera exécutée à chaque réception de message
  clientMqtt.setCallback(onReceiveMsg);
}

void loop() {
  //Relancer l'advertising régulièrement s'il n'y a pas de client connecté
  if (deviceBLEConnected == false)
    BLEDevice::startAdvertising();

  delay(500);

  //Se reconnecter au broker si besoin
  if (clientMqtt.connected() == false) {
    connecter_broker();
  }

  if (etatColonne == 0) {
    //La colonne lumineuse ne s'allume pas
    Serial.println("Colonne non allumée");
    digitalWrite(SIG_PIN, LOW);
  }
  if (etatColonne == 1) {
    //Allumage de la colonne lumineuse
    Serial.println("Colonne allumée en orange");
    digitalWrite(SIG_PIN, HIGH);
  }
  
  //Maintenir la connexion au broker et réceptionner les messages
  clientMqtt.loop();
}