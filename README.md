# 🛠️ Maintenance Prédictive des Machines Tournantes

Bienvenue sur le dépôt du projet **Maintenance Prédictive des Machines Tournantes**, réalisé dans le cadre d’un projet universitaire encadré fictivement par l’entreprise **Emerson** à Armentières.

Ce projet vise à anticiper les défaillances de moteurs industriels grâce à l’analyse des vibrations, permettant ainsi une **maintenance prédictive** fiable et automatisée.

---

## 🎯 Objectifs du projet

- Concevoir un **système complet** permettant de :
  - Mesurer les vibrations de moteurs électriques
  - Analyser les données pour anticiper les défaillances
  - Notifier les responsables en cas de dépassement de seuil
  - Respecter la norme **ISO 10816-1**

---

## 🧠 Fonctionnement global

- Un **ESP32** mesure les vibrations à l’aide d’un capteur **ADXL345**
- Les données sont transmises via **MQTT** à une passerelle **Edge (Raspberry Pi)**
- Les données sont stockées dans une **base MySQL** locale
- Un programme **Node.js** calcule les niveaux globaux de vibration
- Si nécessaire, une **colonne lumineuse** est activée pour alerter
- Un **Cloud** permet d’envoyer des notifications au responsable, via une **application mobile**
- Une **application web** permet de visualiser les graphes des vibrations et leur historique

---

## 📱 Modules développés

### Étudiant 1 (Swann Waeles)
- Système de mesure de vibrations
- Calcul de la **FFT**
- Envoi des données en **JSON** via MQTT
- Intégration avec **Mosquitto** sur la passerelle

### Étudiant 2 (Théo Vanbandon)
- Configuration des ESP via **BLE**
- Stockage dans la **mémoire NVS** des ESP
- Contrôle des colonnes lumineuses
- Intégration et communication en **Node.js**

### Étudiant 3 (Romain Empis)
- Déploiement de la passerelle Edge
- Développement d’une **application web d’analyse** (CodeIgniter)
- Affichage des niveaux instantanés et historiques

### Étudiant 4 (Kylian Dubus)
- Déploiement du **Cloud** (AlwaysData)
- Création d’API REST
- Développement d’une **application de notifications**
- Affichage des données via **Highcharts**

---

## 🖥️ Technologies utilisées

- **ESP32 / Arduino**
- **ADXL345** (capteur de vibrations)
- **MQTT** (Mosquitto)
- **Node.js**
- **MySQL / PHP / CodeIgniter**
- **BLE** (Bluetooth Low Energy)
- **Raspberry Pi**
- **ArduinoJson / ArduinoFFT**
- **Highcharts / HTML / CSS / JS**
- **AlwaysData** (hébergement cloud)

---

## 📊 Schéma de fonctionnement

```plaintext
ESP32 (ADXL345)
     │ (BLE / MQTT)
     ▼
Passerelle Edge (Raspberry Pi)
     │
     ├─ Node.js (calculs, stockage local)
     ├─ Base de données MySQL
     └─ Envoi Cloud (AlwaysData)
           ├─ API REST
           └─ Application Web + Notifications
```

---

## 💡 Norme ISO 10816-1

Le projet suit la norme ISO 10816-1 pour classifier les niveaux de vibration selon :

- La **puissance du moteur**
- Le **type d’assise** (souple ou rigide)
- Des **seuils d’alerte** pour prévenir les défaillances

---

## 📚 Rapport complet

Le rapport complet du projet est disponible sur demande ou dans le dépôt dans le répertoire `Dossier de projet`.

---

## 👨‍🎓 Réalisé par

- Swann Waeles
- Théo Vanbandon
- Romain Empis
- Kylian Dubus

Projet réalisé de **janvier à juin 2023** dans le cadre d’un travail universitaire.

---

## 📄 Licence

Projet fictif réalisé à des fins pédagogiques.
