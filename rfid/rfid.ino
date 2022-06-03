/*
 * Proyecto Tablero Domotico
 * MAGIC HOME
 */

///////////////////////////
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
SoftwareSerial enlace(10,11); // al 3 y al 2 del reproductor respectivamente
DFRobotDFPlayerMini reproductor;
int t=500;
int volumen=25; // de 0 a 30
int espera=2500;
int vv1=0;
int vv2=0;
////////////////////////////
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
//MFRC522 lector1(10,9); //Creamos el objeto para el RC522 (ss SDA,reset)  //Uno
MFRC522 lector1(46,47); //Creamos el objeto para el RC522  //Mega
MFRC522 lector2(48,49);


const int sensor11 = 44;
const int sensor22 =43;

Servo puerta;
Servo seguro;
////////////////////////////
boolean flag1 =0;
boolean flag2 =0;
const int elevator1 = 30;
const int elevator2 = 31;

#define pinPuerta 7 //amarillo
#define pinSeguro 6 //naranja
#define alarma 13
#define sensorHumo A0
#define garage1 32
#define garage2 33
#define sensor1 9
#define sensor2 8.

int s1 = 0;
int s2 = 0;

void setup() {
  Serial.begin(9600); //Iniciamos la comunicación  serial

  pinMode(sensor11, INPUT);
  pinMode(sensor22, INPUT);

    
  SPI.begin();        //Iniciamos el Bus SPI
  lector1.PCD_Init(); // Iniciamos  el MFRC522
  lector2.PCD_Init();
  puerta.attach(pinPuerta);  puerta.write(0);
  seguro.attach(pinSeguro);  seguro.write(90);
  pinMode(alarma,OUTPUT);
  pinMode(sensorHumo,INPUT);
  pinMode(elevator1,OUTPUT);
  pinMode(elevator2,OUTPUT);
  pinMode(garage1,OUTPUT);
  pinMode(garage2,OUTPUT);
  pinMode(sensor1,INPUT);
  pinMode(sensor2,INPUT);
  
  digitalWrite(elevator1,LOW);
  digitalWrite(elevator2,LOW);
  digitalWrite(garage1,LOW);
  digitalWrite(garage2,LOW);
  sonidos();
  reproductor.play(7);  delay(espera);  //Magic Home
/*  
  reproductor.play(1);  delay(espera);  //Elevador Bajando
  reproductor.play(2);  delay(espera);  //Elevador Subiendo
  reproductor.play(3);  delay(espera);  //Luces Apagadas
  reproductor.play(4);  delay(espera);  //Luces Prendidas
  reproductor.play(5);  delay(espera);  //Puerta Abierta
  reproductor.play(6);  delay(espera);  //Puerta Cerrada
  reproductor.play(7);  delay(espera);  //Magic Home
  reproductor.play(8);  delay(espera);  //Alarma
  reproductor.play(7);  delay(espera);  //Magic Home
*/
  Serial.println("Esperando tarjeta:");
  ///////////
        digitalWrite(elevator1,LOW);
        digitalWrite(elevator2,LOW);
}

void loop() {
  s1 = digitalRead(sensor11);
  s2 = digitalRead(sensor22);
  leerTarjetas();
  sensarPeligro();
  delay(100);

  if (s1== LOW && vv1==1)
    {
        digitalWrite(elevator1,LOW);
        digitalWrite(elevator2,LOW);
        vv1==0;
        delay(100);
    }

      if (s2== LOW && vv2==1)
    {
      vv2==0;
        digitalWrite(elevator1,LOW);
        digitalWrite(elevator2,LOW);
        delay(100);
    }

  
}

///////////////////
void leerTarjetas(){
  lector1.PCD_Init(); // Iniciamos  el MFRC522 1
  lector2.PCD_Init(); // Iniciamos  el MFRC522 2
 
  if ( lector1.PICC_IsNewCardPresent()){ //Lector Puerta
    if ( lector1.PICC_ReadCardSerial()){
      //Puerta Peatonal:
      if((lector1.uid.uidByte[3]==152)&&(flag1==0)){
        reproductor.play(5);  delay(espera);  //Puerta Cerrada
        seguro.write(90);
        delay(500);
        lector1.uid.uidByte[3]=0;
        flag1=1;
        puerta.write(90);
        delay(500);
        puerta.write(0);
        Serial.println("Abierto");}
      else if((lector1.uid.uidByte[3]==152)&&(flag1==1)){
        reproductor.play(6);  delay(espera);  //Puerta Cerrada
        puerta.write( 0);
        delay(1000);
        seguro.write( 0);
        lector1.uid.uidByte[3]=0;
        flag1=0;
        Serial.println("Cerrado");}
      delay(500);
    }    lector1.PICC_HaltA();
  }

  if ( lector2.PICC_IsNewCardPresent()){//Lector Ascensor
    if ( lector2.PICC_ReadCardSerial()){
      //Ascensor:
      if((lector2.uid.uidByte[3]==152)&&(flag2==0)){
        reproductor.play(2);  delay(espera);  //Elevador Subiendo
        digitalWrite(elevator1,HIGH);
        digitalWrite(elevator2,LOW);
        vv1=1;
        delay(300);
        
        //digitalWrite(elevator1,LOW);
        //digitalWrite(elevator2,LOW);
        lector1.uid.uidByte[3]=0;
        flag2=1;
        Serial.println("Subiendo");}
      else if((lector2.uid.uidByte[3]==152)&&(flag2==1)&&(sensor2!=LOW)){
        reproductor.play(1);  delay(espera);  //Elevador Bajando
        digitalWrite(elevator1,LOW);
        digitalWrite(elevator2,HIGH);
        vv2=1;
        delay(300);
         //delay(5000);
        //digitalWrite(elevator1,LOW);
        //digitalWrite(elevator2,LOW);
        lector1.uid.uidByte[3]=0;
        flag2=0;
        Serial.println("Bajando ");}
      delay(500);
    }    lector2.PICC_HaltA();
  }
}

////////////////////
void sensarPeligro(){
  int sensorHumo = analogRead(A0);
  if (sensorHumo >= 300){
    reproductor.play(8); delay(500);
    Serial.println("Peligro");
    if (flag1==0){
    digitalWrite(alarma,HIGH);
    seguro.write(90);
    delay(1000);
    puerta.write(90);
    delay(500);
    puerta.write(0);
    }
    lector1.uid.uidByte[3]=0;
    flag1=1;
    Serial.println("Puerta abierta por emergencia");
    //digitalWrite(alarma,HIGH);
    //reproductor.play(8); delay(espera);
  }
  else if (sensorHumo){
    digitalWrite(alarma,LOW);
  }
}

///////////////
void sonidos(){
  enlace.begin(9600);
  if (!reproductor.begin(enlace)) {
    Serial.println(F(" "));    
  }
  Serial.println(F("Reproductor Conectado"));
  reproductor.volume(volumen);
  delay(t);  
}
