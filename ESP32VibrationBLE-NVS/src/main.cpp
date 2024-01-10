#include <SPI.h>
#include "VariablesGlobales.hpp"
#include <ArduinoJson.h>

WiFiClient clientTCP;
PubSubClient clientMqtt(clientTCP);
char out[32768];

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

void connecter_WiFi()
{
  // Connexion au réseau WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.println("Connexion au SSID = " + (String)ssid + " ...");

  // Attendre que la connexion WiFi soit établie
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Nouvelle tentative");
  }
  Serial.println("Connection établie !");
  Serial.println("Adresse MAC ESP = " + WiFi.macAddress());
  Serial.println("Adresse IP ESP = " + WiFi.localIP().toString());
  Serial.println();
}

void connecter_broker()
{
  // Reconnecter le WiFi si besoin
  if (WiFi.status() != WL_CONNECTED)
  {
    connecter_WiFi();
  }
  // Boucle jusqu'à obtenir une connexion
  while (clientMqtt.connected() == false)
  {
    Serial.print("Connexion au serveur MQTT...");
    // le premier paramètre de la méthode connect() doit être un identifiant unique de l'ESP (ici c'est son adresse MAC)
    bool etatConnexionBroker = false;
    if (mqtt_user == "")
    {
      etatConnexionBroker = clientMqtt.connect(WiFi.macAddress().c_str());
    }
    else
    {
      etatConnexionBroker = clientMqtt.connect(WiFi.macAddress().c_str(), mqtt_user, mqtt_password);
    }
    if (etatConnexionBroker == true)
    {
      Serial.println("OK");
    }
    else
    {
      Serial.print("Erreur : ");
      Serial.print(clientMqtt.state());
      Serial.println(" On attend 5 secondes avant de recommencer");
      delay(5000);
    }
  }
}

void calculerFFTunAxe(double *dataReel, double *dataImag, uint16_t nb_echantillons)
{
  // Calcul de la FFT sur un axe (cf exemple de la lib. FFT_04.ino)
  FFT.DCRemoval(dataReel, nb_echantillons);                                   // éliminer la composante continue
  FFT.Windowing(dataReel, nb_echantillons, FFT_WIN_TYP_FLT_TOP, FFT_FORWARD); /*Weigh data */
  FFT.Compute(dataReel, dataImag, nb_echantillons, FFT_FORWARD);              /*Compute FFT */
  FFT.ComplexToMagnitude(dataReel, dataImag, nb_echantillons);                /*Compute magnitudes */
  // normalisation de la magnitude (à modifier suivant le type de fenêtrage)
  // ici pour un fenêtrage FFT_WIN_TYP_FLT_TOP = flat top (Coeff of Salvatore SFT3M Minimum sidelobe 3-term flat top window)
  // cf https://holometer.fnal.gov/GH_FFT.pdf p41 (D.1.4) et p70 (figure 34)
  for (int i = 0; i < nb_echantillons / 2; i++)
  {
    dataReel[i] = (2.0 / nb_echantillons) * dataReel[i] / 0.2810639;
  }
}

void trouverRaiePrincipaleX(double *dataR)
{
  int indPeak = 0;
  float valAccel = 0;
  for (int i = 0; i < (NB_ECHANTILLONS / 2); i++)
  {
    if (valAccel < dataR[i])
    {
      valAccel = dataR[i];
      indPeak = i;
    }
  }
  float frequMax = (float)indPeak * FREQU_ECHANTILLON / NB_ECHANTILLONS;
  indiceRaieX = indPeak;
  accelRaieX = valAccel;
  frequenceRaieX = frequMax;
}

void conversionEnFichierJSON()
{
  DynamicJsonDocument doc(32768);

  doc["mac"] = WiFi.macAddress();
  doc["GUID_moteur"] = GUID_MoteurNVS;
  doc["nb_ech"] = NB_ECHANTILLONS / 2;
  doc["f_ech"] = FREQU_ECHANTILLON;

  JsonObject Xpeaks = doc.createNestedObject("Xpeaks");
  Xpeaks["ind"][0] = indiceRaieX;
  Xpeaks["frequ"][0] = frequenceRaieX;
  Xpeaks["accel"][0] = accelRaieX;

  // JsonObject Ypeaks = doc.createNestedObject("Ypeaks");
  // Ypeaks["ind"][0] = 291;
  // Ypeaks["frequ"][0] = 568.35;
  // Ypeaks["accel"][0] = 57.82;

  // JsonObject Zpeaks = doc.createNestedObject("Zpeaks");
  // Zpeaks["ind"][0] = 293;
  // Zpeaks["frequ"][0] = 572.26;
  // Zpeaks["accel"][0] = 12.26;

  JsonArray Xspectre = doc.createNestedArray("Xspectre");
  for (int i = 0; i < NB_ECHANTILLONS / 2; i++)
  {
    Xspectre.add(round(dataX[i] * 10) / 10);
  }

  JsonArray Yspectre = doc.createNestedArray("Yspectre");
  for (int i = 0; i < NB_ECHANTILLONS / 2; i++)
  {
    Yspectre.add(round(dataY[i] * 10) / 10);
  }

  JsonArray Zspectre = doc.createNestedArray("Zspectre");
  for (int i = 0; i < NB_ECHANTILLONS / 2; i++)
  {
    Zspectre.add(round(dataZ[i] * 10) / 10);
  }
  serializeJson(doc, out);
  Serial.println(out);
}

void ConversionEnAmplitudeEfficace(double *dataR)
{
  for (int i = 0; i < NB_ECHANTILLONS; i++)
  {
    dataR[i] = ((2 * PI * FREQU_ECHANTILLON)/dataR[i])/1000;
    dataR[i] = dataR[i] / sqrt(2);
  }
}

void ViderTableau()
{
  dataX[NB_ECHANTILLONS] = {0};
  imagX[NB_ECHANTILLONS] = {0};
  dataY[NB_ECHANTILLONS] = {0};
  imagY[NB_ECHANTILLONS] = {0};
  dataZ[NB_ECHANTILLONS] = {0};
  imagZ[NB_ECHANTILLONS] = {0};
}

void getValueNVS() {
  ClasseNVS objTest;
  SsidNVS = objTest.LireValeurStr(1); //Lecture du SSID dans la NVS
  Serial.println(SsidNVS);

  PassNVS = objTest.LireValeurStr(2); //Lecture du Password dans la NVS
  Serial.println(PassNVS);

  GUID_MoteurNVS = objTest.LireValeurStr(3); //Lecture de l'identifiant moteur dans la NVS
  Serial.println(GUID_MoteurNVS);

  FrequenceNVS = objTest.LireValeurInt(4) * 1000; //Lecture de la fréquence dans la NVS
  Serial.println(FrequenceNVS);

  //Pour se connecter au WiFi, il faut des variables en const char *
  ssid = &SsidNVS[0];
  Serial.println(ssid);
  password = &PassNVS[0];
  Serial.println(password);

  Serial.println(GUID_MoteurNVS);
  Serial.println(FrequenceNVS);
}

void EnableBLE() {
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
}

//fonction permettant d'activer la configuration ou non
void modeConfiguration() {
  GUID_MoteurNVS = "DEFAULT";
}

void setup() {
  Serial.begin(112500);

  //Récupération des valeurs stockées en NVS
  getValueNVS();

  //fonction permettant d'activer la configuration ou non
  //modeConfiguration();

  //Activer ou non le BLE
  if (GUID_MoteurNVS == "DEFAULT") {
    EnableBLE();
  }

  // Initialisation de la communication SPI
  SPI.end();
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  SPI.setDataMode(SPI_MODE3);
  SPI.setFrequency(5000000); // bus SPI à 5MHz
  adxl.setSpiBit(0);         // 4 wire SPI mode
  adxl.powerOn();            // Power on the ADXL345
  adxl.setRangeSetting(16);  // gamme de mesure jusqu'à 16G
  adxl.setFullResBit(true);  // résolution sur 13 bits (Scale Factor = 4mg/LSB)
  adxl.setRate(3200);        // data rate = 3200Hz (toutes les 312.5 μs)

  // Se connecter au réseau WiFi
  connecter_WiFi();
  // Configurer l'adresse du broker MQTT
  clientMqtt.setServer(mqtt_server_ip, mqtt_server_port);
  clientMqtt.setBufferSize(16384);
}

void loop() {
  if (GUID_MoteurNVS == "DEFAULT") {
    //Relancer l'advertising régulièrement s'il n'y a pas de client connecté
    if (deviceBLEConnected == false) BLEDevice::startAdvertising();
    delay(500);
  }
  
  // Maintenir la connexion au broker MQTT
  // Se reconnecter au broker si besoin
  if (clientMqtt.connected() == false) {
    connecter_broker();
  }
  // Maintenir la connexion au broker et réceptionner les messages
  // doit être appelée régulièrement
  clientMqtt.loop();

  numMesures = 0;
  ViderTableau();
  // Acquisition des mesures d'accélération sur les 3 axes (en m/s2)
  while (numMesures < NB_ECHANTILLONS) {
    currentMicros = micros();
    if (currentMicros - previousMicros >= PERIODE_ECHANTILLON) {
      previousMicros = currentMicros;
      // Acquisition des accélérations sur les 3 axes
      adxl.readAccel(&x, &y, &z);

      // conversion des mesures en mg (cf conf. capteur 4mg/LSB)
      float x_mg = x * 4; // unité mg
      float y_mg = y * 4; // unité mg
      float z_mg = z * 4; // unité mg

      // conversion des mesures en m/s² (1g = 9.81 m/s²)
      float x_ms2 = (x_mg / 1000) * 9.81; // unité m/s^2
      float y_ms2 = (y_mg / 1000) * 9.81; // unité m/s^2
      float z_ms2 = (z_mg / 1000) * 9.81; // unité m/s^2

      // enregistrement des accélérations dans les tableaux "réels"
      dataX[numMesures] = x_ms2;
      dataY[numMesures] = y_ms2;
      dataZ[numMesures] = z_ms2;

      // enregistrement de 0 dans les tableaux "imaginaires"
      imagX[numMesures] = 0;
      imagY[numMesures] = 0;
      imagZ[numMesures] = 0;

      numMesures += 1;
    }
  }

  // Calcul de la FFT sur les 3 axes
  calculerFFTunAxe(dataX, imagX, NB_ECHANTILLONS);
  calculerFFTunAxe(dataY, imagY, NB_ECHANTILLONS);
  calculerFFTunAxe(dataZ, imagZ, NB_ECHANTILLONS);

  // Trouver la raie principale de chaque axe
  trouverRaiePrincipaleX(dataX);

  // Conversion des valeurs en mm/s RMS
  // ConversionEnAmplitudeEfficace(dataX);
  // ConversionEnAmplitudeEfficace(dataY);
  // ConversionEnAmplitudeEfficace(dataZ);

  // Convertir les données en JSON
  conversionEnFichierJSON();

  // Envoyer les données en MQTT
  clientMqtt.publish(monPublishTopic, out);
  delay(FrequenceNVS);
}