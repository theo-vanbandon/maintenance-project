var topicAlerte = "Alerte";
var topicSpectre = "SpectreVib";
let zoneA;
let zoneB;
let zoneC;
let zoneD;
let zoneAenvoyer;
let GUIDMachine = "";

//https://www.npmjs.com/package/mysql
//Connexion à la BDD + récupération de la valeur des zones
function recupZone() {
    var mysql = require('mysql');
    var connection = mysql.createConnection({
        host: '172.17.129.128',
        user: 'UserVibration',
        password: 'vibrations',
        database: 'BDD_Vibration'
    });

    connection.connect(function (err) {
        if (err) {
            console.error('error connecting: ' + err.stack);
            return;
        }
        console.log('connected as id ' + connection.threadId);
        connection.query('SELECT seuil FROM niveau_norme', function (error, results, fields) {
            if (error) throw error;
            //connected!
            console.log(results);
            zoneA = results[0].seuil;
            zoneB = results[1].seuil;
            zoneC = results[2].seuil;
            zoneD = results[3].seuil;
            for (let i = 0; i < results.length; i++) {
                console.log(results[i].seuil);
            }
            connection.end();
        });
    });
}


//Connexion à la BDD + récupération de l'identifiant de la machine correspondant aux moteurs
function recupGUIDmachine(prmGUIDMoteur) {
    var mysql = require('mysql');
    var connection = mysql.createConnection({
        host: '172.17.129.128',
        user: 'UserVibration',
        password: 'vibrations',
        database: 'BDD_Vibration'
    });

    connection.connect(function (err) {
        if (err) {
            console.error('error connecting: ' + err.stack);
            return;
        }
        console.log('connected as id ' + connection.threadId);
        connection.query('SELECT idMachine, GUID_machine FROM moteur, machine WHERE moteur.GUID_moteur = "' + prmGUIDMoteur + '" AND idMachine = machine.id;', function (error, results, fields) {
            if (error) throw error;
            //connected!
            console.log(results);
            GUIDMachine = results[0].GUID_machine;
            console.log(GUIDMachine);
            connection.end();
            publishMQTT(topicAlerte, zoneAenvoyer);
        });
    });
}

//Connexion à la BDD + Ajout des valeurs
function EnregistrementVib(prmMoyX, prmMoyY, prmMoyZ) {
    var mysql = require('mysql');
    var connection = mysql.createConnection({
        host: '172.17.129.128',
        user: 'UserVibration',
        password: 'vibrations',
        database: 'BDD_Vibration'
    });

    connection.connect(function (err) {
        if (err) {
            console.error('error connecting: ' + err.stack);
            return;
        }
        console.log('connected as id ' + connection.threadId);
        connection.query('INSERT INTO `vibration`(`spectreX_json`, `spectreY_json`, `spectreZ_json`, `frequEch`, `nbEchantillon`, `niveau_globalX`, `niveau_globalY`, `niveau_globalZ`, `idMoteur`) VALUES ("Test", "Test", "Test", 9, 9, ' + prmMoyX + ',' + prmMoyY + ',' + prmMoyZ + ', 1);', function (error, results, fields) {
            if (error) throw error;
            //connected!
            connection.end();
        });
    });
}



//http://www.steves-internet-guide.com/using-node-mqtt-client/
//Fonction pour publier dans le topic "Alerte"
function publishMQTT(prmTopic, prmZone) {
    var options = {
        retain: true,
        qos: 1
    };

    let myObj = {zone: prmZone, guid: GUIDMachine};
    let myString = JSON.stringify(myObj);
    console.log(myString);
    console.log();

    var timer_id = setInterval(function () { publish(prmTopic, myString, options); }, 5000);

    var mqtt = require('mqtt');
    var count = 0;
    var client = mqtt.connect("mqtt://172.17.129.128", { clientId: "mqttjs01" });
    console.log("connected flag  " + client.connected);

    //handle incoming messages
    client.on('message', function (prmTopic, myString, packet) {
        console.log("message is " + myString);
        console.log("topic is " + prmTopic);
    });


    client.on("connect", function () {
        console.log("connected " + client.connected);
    })
    //handle errors
    client.on("error", function (error) {
        console.log("Can't connect" + error);
        process.exit(1)
    });

    //publish
    function publish(prmTopic, myString, options) {
        console.log("publishing", myString);

        if (client.connected == true) {
            client.publish(prmTopic, myString, options);
        }
        count += 1;
        if (count == 2) //ens script
            clearTimeout(timer_id); //stop timer
        client.end();
    }
}



function subscribe(prmTopic) {
    //Lien site: https://gist.github.com/smching/3582d2fbae417fc919e23b4e61c036d1
    var mqtt = require('mqtt');
    var Topic = prmTopic; //Subscribe to all topics
    var Broker_URL = 'mqtt://172.17.129.128';

    var options = {
        clientId: 'MyMQTT',
        port: 1883,
        keepalive: 60
    };

    var client = mqtt.connect(Broker_URL, options);
    client.on('connect', mqtt_connect);
    client.on('reconnect', mqtt_reconnect);
    client.on('error', mqtt_error);
    client.on('message', mqtt_messsageReceived);
    client.on('close', mqtt_close);

    function mqtt_connect() {
        console.log("Connecting MQTT");
        client.subscribe(Topic, mqtt_subscribe);
    }

    function mqtt_subscribe(err, granted) {
        console.log("Subscribed to " + Topic);
        if (err) { console.log(err); }
    }

    function mqtt_reconnect(err) {
        console.log("Reconnect MQTT");
        if (err) { console.log(err); }
        client = mqtt.connect(Broker_URL, options);
    }

    function mqtt_error(err) {
        console.log("Error!");
        if (err) { console.log(err); }
    }

    function after_publish() {
        //do nothing
    }

    function mqtt_messsageReceived(topic, message, packet) {
        console.log('Topic: ' + topic + ' | Message: ' + message);
        let json = JSON.parse(message);

        let GUID_moteur = json.GUID_moteur; //Récupération ID Moteur
        console.log("GUID du moteur: " + GUID_moteur);

        let AddMAC = json.mac;      //Récupération de l'adresse MAC
        console.log("Adresse MAC: " + AddMAC);

        let nb_ech = json.nb_ech;   //Récupération du nombre d'échantillon
        console.log("Nombre d'échantillon:  " + nb_ech);

        let f_ech = json.f_ech;     //Récupération de la fréquence d'échantillonement
        console.log("Fréquence d'échantillonement: " + f_ech);

        let XpeaksInd = json.Xpeaks.ind[0];
        let XpeaksFrequ = json.Xpeaks.frequ[0];
        let XpeaksAccel = json.Xpeaks.accel[0];
        console.log("Xpeaks indice: " + XpeaksInd);
        console.log("Xpeaks fréquence: " + XpeaksFrequ);
        console.log("Xpeaks accélération: " + XpeaksAccel);

        //Calcul des niveaux globaux
        let sommeX = 0;
        let sommeY = 0;
        let sommeZ = 0;

        for (let i = 0; i < nb_ech; i++) {
            sommeX = sommeX + (json.Xspectre[i] * json.Xspectre[i]);
            sommeY = sommeY + (json.Yspectre[i] * json.Yspectre[i]);
            sommeZ = sommeZ + (json.Zspectre[i] * json.Zspectre[i]);
        }

        console.log();
        console.log("Somme de l'axe X: " + sommeX);
        console.log("Somme de l'axe Y: " + sommeY);
        console.log("Somme de l'axe Z: " + sommeZ);
        let moyX = Math.round(Math.sqrt(sommeX) * 100) / 100;
        let moyY = Math.round(Math.sqrt(sommeY) * 100) / 100;
        let moyZ = Math.round(Math.sqrt(sommeZ) * 100) / 100;
        console.log("Moyenne de l'axe X: " + moyX);
        console.log("Moyenne de l'axe Y: " + moyY);
        console.log("Moyenne de l'axe Z: " + moyZ);
        console.log();

        //Comparaison avec la zone
        if ((moyX >= zoneA) && (moyX < zoneB)) zoneAenvoyer = "A";
        if ((moyY >= zoneA) && (moyY < zoneB)) zoneAenvoyer = "A";
        if ((moyZ >= zoneA) && (moyZ < zoneB)) zoneAenvoyer = "A";

        if ((moyX >= zoneB) && (moyX < zoneC)) zoneAenvoyer = "B";
        if ((moyY >= zoneB) && (moyY < zoneC)) zoneAenvoyer = "B";
        if ((moyZ >= zoneB) && (moyZ < zoneC)) zoneAenvoyer = "B";

        if ((moyX >= zoneC) && (moyX < zoneD)) zoneAenvoyer = "C";
        if ((moyY >= zoneC) && (moyY < zoneD)) zoneAenvoyer = "C";
        if ((moyZ >= zoneC) && (moyZ < zoneD)) zoneAenvoyer = "C";

        if (moyX >= zoneD) zoneAenvoyer = "D";
        if (moyY >= zoneD) zoneAenvoyer = "D";
        if (moyZ >= zoneD) zoneAenvoyer = "D";

        //Envoi des données dans la BDD
        EnregistrementVib(moyX, moyY, moyZ);

        //Récup de l'identifiant de la machine correspondant au moteur
        recupGUIDmachine(json.GUID_moteur);
    }

    function mqtt_close() {
        console.log("Close MQTT");
    }
}

recupZone();    //Récupération du seuil des zones
subscribe(topicSpectre);    //Subscribe au topic des spectres