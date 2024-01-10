// #include <Arduino.h>
// int SIG_PIN = 32;
// bool allumer = false;
// int i = 0;

// void setup() {
//   Serial.begin(9600);
//   pinMode(SIG_PIN, OUTPUT);
// }

// void loop() {
//   if ((i % 2) == 0) {
//     allumer = true;
//   } else {
//     allumer = false;
//   }
//   //Lampe éteinte si allumer = false
//   if (allumer == false) {
//     digitalWrite(SIG_PIN, LOW);
//     Serial.println("Lampe éteinte");
//   }
//   //Lampe allumée si allumer = true
//   if (allumer == true) {
//     digitalWrite(SIG_PIN, HIGH);
//     Serial.println("Lampe allumée");
//   }
//   i++;
//   delay(10000);
// }






#include <Arduino.h>
int SIG_PIN = 32;

void setup() {
  Serial.begin(9600);
  pinMode(SIG_PIN, OUTPUT);
}

void loop() {
  //Lampe allumée
  digitalWrite(SIG_PIN, HIGH);
  Serial.println("Lampe allumée");
  delay(2000);

  //Lampe éteinte
  digitalWrite(SIG_PIN, LOW);
  Serial.println("Lampe éteinte");
  delay(2000);
}