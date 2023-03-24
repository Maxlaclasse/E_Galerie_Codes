#include <Arduino.h>
#include <HardwareSerial.h>
#include <WiFi.h>                                                                                                                                                                   a aa  a a a a a a ia  a       aa  a   a n a a a a ac  a aaaaaaaaaaaaaaaa  a aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"a  aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa aaaaaaaaaa"""           aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa a a a     a         a       a       a aaaaaaa aa    aa  a aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa aaaaaaaaaaaaaaaaaaaaaaa"""""""  aaaaaaaaaaaaaaaaaaa a a aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa  aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa  aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa  aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa aaaaaaaaaaa aaaaaaaaaa  a a a a a a aaaaaaaaaaaaaaaaaaaaaaaaaa  aaaaaaaa      a aa    a   aaaa      aa      aa    a²  a a a   a   a²² lude <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WebSocketsClient.h>
#include <WebServer.h>
#include <WiFiClient.h>

#define SERVER_IP "172.20.10.2"
#define SERVER_PORT 8084

const char* ssid = "iPhone de Maxime";  // Mettre le ssid de votre réseau Wifi
const char* password = "maximerouillard";   // Mettre le mot de passe de votre réseau Wifi

#define LED 2
#define Laser 4
#define LEDPower 15
#define LEDWIFI1 2
#define LEDWIFI2 
int LaserValue = 0;
#define brochePWM1  13
#define brochePWM2  12
#define brochePWM3  14
#define brochePWM4  27
#define ENCODEURG   34
#define ENCODEURD   35
#define pi 3.1415926535897932384626433832795
int pwmChannel1 = 0;
int pwmChannel2 = 1;
int frequence = 40000;//80
int resolution = 8;
int PWM1 = 32;
int PWM2 = 33;

int etatPWM1 = 0;
int etatPWM2 = 0;
int etatPWM3 = 0;
int etatPWM4 = 0;
int Stop = 0;
int interupt = 0;

const byte TRIGGER_PIN = 5; // Broche TRIGGER
const byte ECHO_PIN = 18;    // Broche ECHO
const unsigned long MEASURE_TIMEOUT = 25000UL; // 25ms = ~8m à 340m/s
const float SOUND_SPEED = 340.0 / 1000;

char Xs[20];
char Ws[20];
char Cs[20];
char xs[20];
char ws[20];
char cs[20];

//ultrason
float distance_mm;
long measure;

//Encodeur
//Moteur gauche:
int comptG = 0;
int comptGr = 0; 
int a;
float dl = 0; // déplacement roue gauche
float deltaTICKG;

//Moteur droite:
int comptD = 0;
int comptDr = 0;
int b;
float dr = 0; // déplacment roue droite
float deltaTICKD;

float R = 3.3; // rayon des roues
float N = 1130; // nombre de tick par revolution
float dc = 0; // déplacement centre robot
float L = 24.7; //distance entre les roues

//position robot
float x = 0;
float X = 0;
float y = 0;
float Y = 0;
float phi = 0 ;
float PHI = 0 ; 

WebSocketsClient webSocket;


void onWebSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

 switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("Disconnected!\n");
      break;
    case WStype_CONNECTED:
      Serial.printf("Connected to url: %s\n", payload);
      break;
    case WStype_TEXT:
      Serial.printf("Received message: %s\n", payload);
      char pay[length+1]; 
      char avant[10] = "forward";
      char arriere[10]= "backward";
      char gauche[10]= "left";
      char droite[10]= "right";
      char stop[10]= "stop";
      strncpy(pay,(char*)payload,length);
      pay[length]='\0';
      
      if (strcmp(pay, avant) == 0)
      { 
        etatPWM1 = 1;
      }
      else if(strcmp(pay, arriere) == 0){
        
        etatPWM1 = 2;
         
      }
      else if(strcmp(pay, gauche) == 0){

        etatPWM1 = 3;
         
      }
      else if(strcmp(pay, droite) == 0){
        etatPWM1 = 4;

         
      }
      else if(strcmp(pay, stop) == 0){
        etatPWM1 = 5;
         
      }
      break;
  }
}

void transition() {
  delay(100);
  digitalWrite(brochePWM1, 0);
  digitalWrite(brochePWM2, 0);
  digitalWrite(brochePWM3, 0);
  digitalWrite(brochePWM4, 0);
  delay(100);
  ledcWrite(pwmChannel1, 220);
  ledcWrite(pwmChannel2, 220);
}

void transitionFin() {
  for (int i = 150; i < 220; i++) {
    delay(10);
    ledcWrite(pwmChannel1, i);
    ledcWrite(pwmChannel2, i);
  }
}

void avance() {
  digitalWrite(brochePWM1, 1);
  digitalWrite(brochePWM2, 0);
  digitalWrite(brochePWM3, 1);
  digitalWrite(brochePWM4, 0);
}
void recule() {
  digitalWrite(brochePWM1, 0);
  digitalWrite(brochePWM2, 1);
  digitalWrite(brochePWM3, 0);
  digitalWrite(brochePWM4, 1);
}
void gauche() {
  digitalWrite(brochePWM1, 1);
  digitalWrite(brochePWM2, 0);
  digitalWrite(brochePWM3, 0);
  digitalWrite(brochePWM4, 1);
}
void droite() {
  digitalWrite(brochePWM1, 0);
  digitalWrite(brochePWM2, 1);
  digitalWrite(brochePWM3, 1);
  digitalWrite(brochePWM4, 0);
}
void arret() {
  digitalWrite(brochePWM1, 0);
  digitalWrite(brochePWM2, 0);
  digitalWrite(brochePWM3, 0);
  digitalWrite(brochePWM4, 0);
}

void laser() {
  digitalWrite(Laser, HIGH);
}

void gestionPWMS() {
  if (Stop == 1) {
    arret();
  }
  else if (etatPWM1 == 1) {
    transition();
    avance();
    transitionFin();
  }
  else if (etatPWM2 == 1) {
    transition();
    recule();
    transitionFin();
  }
  else if (etatPWM3 == 1) {
    transition();
    gauche();
    transitionFin();
  }
  else if (etatPWM4 == 1) {
    transition();
    droite();
    transitionFin();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n");
  WiFi.begin(ssid, password);  // Initialisation du Wifi
  Serial.print("Attente de connexion ...");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\n");  // Affichage "Connexion établie" et de l'adresse IP
  Serial.println("Connexion etablie !");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());

  pinMode(brochePWM1, OUTPUT);
  pinMode(brochePWM2, OUTPUT);
  pinMode(brochePWM3, OUTPUT);
  pinMode(brochePWM4, OUTPUT);
  ledcSetup(pwmChannel1, frequence, resolution);
  ledcSetup(pwmChannel2, frequence, resolution);
  ledcAttachPin(PWM1, pwmChannel1);
  ledcAttachPin(PWM2, pwmChannel2);
  pinMode(TRIGGER_PIN, OUTPUT);
  digitalWrite(TRIGGER_PIN, LOW); // La broche TRIGGER doit être à LOW au repos
  pinMode(ECHO_PIN, INPUT);

  pinMode(ENCODEURG, INPUT);
  pinMode(ENCODEURD, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(LEDPower, OUTPUT);
  pinMode(LEDWIFI1, OUTPUT);
  pinMode(Laser, OUTPUT);
  digitalWrite(LED, HIGH);

  attachInterrupt(ENCODEURG, odometrieG, RISING);
  attachInterrupt(ENCODEURD, odometrieD, RISING);
  

  webSocket.begin(SERVER_IP, SERVER_PORT, "/");
  webSocket.onEvent(onWebSocketEvent);
}

void loop() {

  webSocket.loop();
  delay(50);
  webSocket.sendTXT("ok envoie");

  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  measure = pulseIn(ECHO_PIN, HIGH, MEASURE_TIMEOUT);
  distance_mm = measure / 2.0 * SOUND_SPEED;


  //if (distance_mm < 100)
  //{ 
  if (etatPWM1 == 1) {
      //Serial.printf("ça va ou koi?");
      gcvt(distance_mm, 10, Xs);
      ledcWrite(pwmChannel1, 173);
      ledcWrite(pwmChannel2, 170);
      avance();
  
      //webSocket.loop();
  
      odometrie();
  
  
    }
  
    else if (etatPWM1 == 2) {
      Serial.printf("selem");
      digitalWrite(TRIGGER_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIGGER_PIN, LOW);
      ledcWrite(pwmChannel1, 173);
      ledcWrite(pwmChannel2, 170);
  
      recule();
  
      //webSocket.loop();
  
      odometrie();
  
  
  
    }
  
    else if (etatPWM1 == 3) {
      Serial.printf("selem");
      digitalWrite(TRIGGER_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIGGER_PIN, LOW);
      ledcWrite(pwmChannel1, 173);
      ledcWrite(pwmChannel2, 170);
      
      gauche();
  
      //webSocket.loop();
  
      odometrie();
    }
  
    else if (etatPWM1 == 4) {
      digitalWrite(TRIGGER_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIGGER_PIN, LOW);
      ledcWrite(pwmChannel1, 173);
      ledcWrite(pwmChannel2, 170);
      droite();
  
      //webSocket.loop();
  
      odometrie();
  
    }
  
    else if (etatPWM1 == 5) {
      arret();
      
      //webSocket.loop();
  
      odometrie();
  


    }
  
    else if(LaserValue == 1) {
      laser();
    }
  //}

  /*else 
  {
      Serial.printf("j'ai capté");
      recule();
      delay(1000);
      
    
    gauche();
    delay(1000);

  }*/


}

void odometrieG()
{
  if (etatPWM1 == 1){
  comptG ++;
  comptGr = 0; 
  //gcvt(comptG, 10, Xs);
  //gcvt(comptGr, 20, Ws);
  }  
  
  else if (etatPWM2 == 1){
    comptG --;
    comptGr = 0;
  }
  else if (etatPWM3 == 1){
    comptGr --; 
    comptG = 0;
  }
  else if (etatPWM4 == 1) {
    comptGr ++; 
    comptG = 0; 
    //gcvt(comptGr, 20, xs);
    //gcvt(comptG, 20, ws);
  }

}

void odometrieD()
{
  if (etatPWM1 == 1){
  comptD ++;
  comptDr = 0;
  //gcvt(comptD, 10, Ws);
  }  
  
  else if (etatPWM2 == 1){
    comptD --;
    comptDr = 0; 
  }
  else if (etatPWM3 == 1){
    comptDr ++ ; 
    comptD = 0 ; 
  }
  else if (etatPWM4 == 1) {
    comptDr --; 
    comptD = 0; 
  }

}

void odometrie()
{

 if ((etatPWM4 == 1) || (etatPWM3 == 1)){

  deltaTICKG = comptGr / N; // tour g 
  deltaTICKD = comptDr / N;

  dl = 2 * pi * R * deltaTICKG;
  dr = 2 * pi * R * deltaTICKD;

  

  X = x ;
  Y = y ;
  phi = (dr - dl) / L  + PHI;
  gcvt(X, 20, xs);
  gcvt(Y, 20, ws);
  gcvt(phi, 20, cs);

  }

  else if ((etatPWM1 == 1) || (etatPWM2 == 1 )) {

    dl = 2 * pi * R * comptG / N ;
    dr = 2 * pi * R * comptD / N;
    //gcvt(dl,20,Xs);
    //gcvt(dr,20,Ws);
    
    dc = (dr + dl) / 2;
    //gcvt(dc, 20, Cs);

    PHI = phi; 
    x = dc * cos(phi) + X ;
    y = dc * sin(phi) + Y ;

    gcvt(X, 10, xs);
    gcvt(x, 10, Xs);
    gcvt(y, 10, Ws);
    gcvt(phi, 10, Cs);
    
  }
   
  else if (Stop ==1){
    
    dl = 2 * pi * R * comptG / N ;
    dr = 2 * pi * R * comptD / N;
    
    dc = (dr + dl) / 2;

    deltaTICKG = comptGr / N; // tour g 
    deltaTICKD = comptDr / N;

    dl = 2 * pi * R * deltaTICKG;
    dr = 2 * pi * R * deltaTICKD;

    phi = (dr - dl) / L  + PHI;

    x = dc * cos(phi) + X ;
    y = dc * sin(phi) + Y ;

    gcvt(X, 10, xs);
    //gcvt(x, 10, Xs);
    //gcvt(y, 10, Ws);
    gcvt(phi, 10, Cs);
  }
}