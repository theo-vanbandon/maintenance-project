//UUID du service et des caractéristiques (créés sur l'ESP)
const SERVICE_CONFIG_ESP = "4fafc201-1fb5-278d-8fcc-c5c9c331914b";
const CARACT_CONFIG_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

//Référence JS vers le device BLE sur lequel on est connecté
let bluetoothDevice = null;

//Variable récupération valeur du formulaire
let typeObjet;
let SSID;
let pass;
let frequence;
let UUIDMoteur
let UUIDMachine;

//Gestion du clic sur le bouton d'envoi
const btnEnvoi = document.getElementById("btnEnvoi");   // référence vers le bouton d'envoi
const input_SSID = document.getElementById("input_SSID"); //Formulaire SSID
const input_Password = document.getElementById("input_Password"); //Formulaire Password
const input_TypeObjet = document.getElementById("typeObjet"); //Formulaire TypeObjet
const Desc1 = document.getElementById('Desc1'); //Description formulaire
const paraform1 = document.getElementById('paraform1'); //Formulaire 1
const paraform2 = document.getElementById('paraform2'); //Formulaire 2
const paraform3 = document.getElementById('paraform3'); //Formulaire 3
const opt1 = document.getElementById('opt1');
const opt2 = document.getElementById('opt2');
const opt3 = document.getElementById('opt3');
const p_text = document.getElementById("text");




//Gestion du click sur le bouton d'envoi
btnEnvoi.addEventListener('click', function clickBtn (e) {
  e.preventDefault();

  if (input_TypeObjet.value == "vib") {
    typeObjet = input_TypeObjet.value;
    SSID = input_SSID.value;
    pass = input_Password.value;
    FormulaireVibration();
  } else if (input_TypeObjet.value == "col"){
    typeObjet = input_TypeObjet.value;
    SSID = input_SSID.value;
    pass = input_Password.value;
    FormulaireColonne();
  }
});




//Formulaire pour un ESP Vibration
function FormulaireVibration() {
  document.formulaire.reset();  //Reset des valeurs du formulaire

  //Modification texte de la description du formulaire pour l'ESP Vibration
  let textPara = "Voici le formulaire pour configurer un ESP Vibration";
  Desc1.textContent = textPara;

  //Modification texte pour la fréquence
  let textFormulaire1 = "Quelle est la fréquence des mesures ? (en s)";
  paraform1.textContent = textFormulaire1;
  opt1.textContent = "Choisissez la fréquence";

  //Mofication des options + suppression de l'attribut value
  let option2 = 30;
  opt2.textContent = option2;
  opt2.removeAttribute("value");

  let option3 = 60;
  opt3.textContent = option3;
  opt3.removeAttribute("value");

  //Ajout d'option pour arriver à 1000
  for (let i = 3; i <= 10; i++) {
    const newOpt = document.createElement('option');
    newOpt.id = "opt" + i;
    newOpt.textContent = i * 30;
    input_TypeObjet.append(newOpt);
  }

  //Modification du texte pour l'identifiant du moteur
  let textFormulaire2 = "Identification du moteur";
  paraform2.textContent = textFormulaire2;
  input_SSID.placeholder = "Veuillez insérer l'indentifiant du moteur";

  //Suppression de la dernière case du formulaire
  const deleteForm = document.querySelectorAll('#form #div3');
  for (let i = 0; i < deleteForm.length; i++) {
    deleteForm[i].remove();
  }

  btnEnvoi.textContent = "Connexion à l'ESP32";

  btnEnvoi.addEventListener('click', function (){
    CreateJSON();
  });
}




//Formulaire pour un ESP Colonne
function FormulaireColonne() {
  document.formulaire.reset();  //Reset des valeurs du formulaire

  //Modification texte de la description du formulaire pour l'ESP Colonne
  let textPara = "Voici le formulaire pour configurer un ESP Colonne";
  Desc1.textContent = textPara;

  //Suppression de la première case du formulaire
  const deleteForm = document.querySelectorAll('#form #div1');
  for (let i = 0; i < deleteForm.length; i++) {
    deleteForm[i].remove();
  }

  //Suppression de la deuxième case du formulaire
  const deleteForm2 = document.querySelectorAll('#form #div2');
  for (let i = 0; i < deleteForm2.length; i++) {
    deleteForm2[i].remove();
  }

  //Modification du texte pour l'identifiant de la machine
  let textFormulaire = "Identification de la machine";
  paraform3.textContent = textFormulaire;
  input_Password.placeholder = "Veuillez insérer l'indentifiant de la machine";
  input_Password.type = "";

  btnEnvoi.textContent = "Connexion à l'ESP32";

  btnEnvoi.addEventListener('click', function (){
    CreateJSON();
  });
}




//Fonction créant le JSON
function CreateJSON() {
  //Récupération de la valeur de la fréquence
  frequence = input_TypeObjet.value;
  //Récupération de l'identifiant de la machine
  UUIDMachine = input_Password.value;
  //Récupération de l'identifiant du moteur
  UUIDMoteur = input_SSID.value;

  //JSON
  let myObj;
  if (typeObjet == "vib") {
    myObj = {type: typeObjet, SSIDPointAcces: SSID, Password: pass, Frequence: frequence, IDMoteur: UUIDMoteur};
  }
  if (typeObjet == "col") {
    myObj = {type: typeObjet, SSIDPointAcces: SSID, Password: pass, IDMachine: UUIDMachine};
  }
  let myString = JSON.stringify(myObj);
  console.log(myString);

  connexionBLE(myString);
}




//Recherche des devices BLE (seulement ceux qui implémentent SERVICE_CONFIG_ESP
//Connexion automatique au device BLE choisi 
//et lecture de sa caractéristique CARACT_CONFIG_UUID, décodage puis affichage
function connexionBLE(prmMyString) {
  if (bluetoothDevice == null) {
    var nav = navigator;
    if (nav.bluetooth && nav.bluetooth.requestDevice) {
      //Afficher seulement les devices qui implémentent le service SERVICE_CONFIG_ESP
      nav.bluetooth.requestDevice({ filters: [{ services: [SERVICE_CONFIG_ESP] }] })
        .then(device => {
          // Quand un device est sélectionné
          console.log("requestDevice", device);
          console.log("device name", device.name);
          // Enregistrer la référence vers le device BLE
          bluetoothDevice = device;
          // et enregistrement de la fonction de callback pour gérer la déconnexion
          bluetoothDevice.addEventListener('gattserverdisconnected', onDeviceDisconnected);
          // Se connecter au serveur GATT du device
          return device.gatt.connect();
        })
        .then(server => {
          console.log("Connecté sur le serveur GATT", server);
          // Accéder au service...
          return server.getPrimaryService(SERVICE_CONFIG_ESP);
        })
        .then(service => {
          console.log("Accès au service", service);
          // Accéder à la caractéristique CARACT_CONFIG_UUID
          return service.getCharacteristic(CARACT_CONFIG_UUID);
        })
        .then(characteristic => {
          console.log("Accès à la caractéristique", characteristic);
          // Lire la valeur de la caractéristique
          let encoder = new TextEncoder('utf-8');
          let sendMsg = encoder.encode(prmMyString);
          return characteristic.writeValue(sendMsg);
        })
        .then(value => {
          // Décoder et afficher la valeur de la caractéristique
          console.log("Lecture de la valeur de la caract", value);
          let decoder = new TextDecoder('utf-8');
          let name = decoder.decode(value);
          console.log('value is ' + name);
          p_text.textContent = name;
          FormulaireValide();
        })
        .catch(error => { console.log("webbluetooth error", error); });
    }
  } else {
    console.log("Device BLE déjà connecté");
  }
}




//Formulaire valide
function FormulaireValide() {
  //Suppression du formulaire
  const form = document.querySelectorAll('.formulaire');
  for (let i = 0; i < form.length; i++) {
    form[i].remove();
  }

  //Ajout paragraphe validation
  const para = document.getElementById('text');
  para.textContent = "Formulaire valide !";
}




// fonction de callback appelée quand le device BLE se déconnecte
function onDeviceDisconnected(event) {
  // Object event.target is Bluetooth Device getting disconnected.
  console.log("Device BLE déconnecté", event.target);
  p_text.textContent = "";
}