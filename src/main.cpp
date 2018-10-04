#include <Arduino.h>
#include <Servo.h>
#include "Hbro.h"
#include "CurrentSensor30A.h"

int pumpPWMPin = 3; //bestemmer forrovehastigheten på pumpe motoren
int pumpEnablePin = 16; //enabler forover. Kan være høy hele siden PWM styrer hastighet. Høy er på
int lukeForPWMPin = 9; //gir forrover signal til H-boren, bakover PWM må være 0 for at den skal fungere
int lukeBackPWMPin =6; //gir bakover signal til H-broen, forrover PWM må være 0 for at den skal fungere

int pumpButtonPin  = 8; //kanpp for å starte pumpen, når den er inne kjører pumpen
int lidOpenButtonPin = 7;
int lidCloseButtonPin = 4;
int highPin = 5; //bare for å ta ut 5V
int currentSensorPin = A0;
float current = 0;
int openFlag = 0; //er 1 når lokket er oppe og 0 når det er alt annet

int vakuumCurrentSensorPin = A3;
float vCurrent = 0;
int vCurrentPeakFlag = 0;
int vakuumDoneFlag = 0;
int vakuumMotorRunningFlag = 0;

Hbro vakuumMotor(pumpPWMPin , 0, pumpEnablePin, 14); //setter opp et objekt for pumpen
Hbro lukeMotorer(lukeForPWMPin, lukeBackPWMPin, 0, 0); //setter opp et objekt for luken, her blir begge motorene kjørt i paralell
CurrentSensor30A lukeCurrent(currentSensorPin);
CurrentSensor30A vakuumCurrent(vakuumCurrentSensorPin);

void setup() {
  Serial.begin(9600);
  vakuumMotor.setupFuction(); //setter inputs og outputs
  //vakuumMotor.enableDebugging(); //skriver info til konsoll så man kan overvåke
  lukeMotorer.setupFuction(); //setter inputs og outputs
  //lukeMotorer.enableDebugging(); //skriver info til konsoll så man kan overvåke

  pinMode(pumpButtonPin, INPUT); //tar inn signal fra knapp. 5V er høy 0V er lav
  pinMode(lidOpenButtonPin, INPUT); //tar inn signal fra knapp. 5V er høy 0V er lav
  pinMode(lidCloseButtonPin, INPUT); //tar inn signal fra knapp. 5V er høy 0V er lav
  pinMode(highPin, OUTPUT); //pinne for 5V
  digitalWrite(highPin, HIGH); //pinne for 5V

  //no setup for analog input needed
}


void loop() {

  if(digitalRead(pumpButtonPin) == HIGH && !vakuumDoneFlag){ //hvis knappen for pumpen er inne skal den på, med mindre vakuum er ferdig
    Serial.println("Pump on");
    vakuumMotor.setSpeed(1,250);
	vakuumMotorRunningFlag = 1; // Indikerer at vakuum-motoren er på
  }
  else{
    //Serial.println("Pump off");
    vakuumMotor.setSpeed(1,0); //hvis knappen for pumpen er ute skal den av
	vakuumMotorRunningFlag = 0; // Indikerer at vakuum-motoren er av
  }

  if(digitalRead(lidOpenButtonPin)==HIGH){ //lokket kjører opp så lenge knappen er inne og den ikke er på toppen
    Serial.println("Open lid");
    if(current < 0.80 && openFlag == 0){ //lokket går bare opp hvis det ikke er oppe fra før eller det spenningen øker fra at det har nådd toppen
      lukeMotorer.setSpeed(0,250);
    }
    else{
        lukeMotorer.setSpeed(0, 0);
        openFlag = 1; //flagge er satt til 1 for å vise at lokket er oppe
		vakuumDoneFlag = 0; // Lokket er oppe, derfor kan vakuum startes igjen
    }
  }
  else if(digitalRead(lidCloseButtonPin)==HIGH){ //lokket kjører ned så lenge kanppen er inne
    Serial.println("Close lid");
    openFlag = 0; //siden luken har gått ned er den ikke åpen lengre
    if(current > 0.1){ //hvis luken er på bunnen skal den gi ekstra gass for å trykke ned lokket
      lukeMotorer.setSpeed(1,250);
      Serial.println("Boost!");
    }
    else{
        lukeMotorer.setSpeed(1, 100); //vanlig lukking
    }

  }
  else{
    //Serial.println("neutral");
    lukeMotorer.setSpeed(0,0);
  }
  //Serial.print("Voltage: ");
  current = 0.95*current + 0.05*lukeCurrent.getCurrent();
  vCurrent = 0.95*vCurrent + 0.05*vakuumCurrent.getCurrent();

  if(abs(vCurrent) > 1.45 && !vCurrentPeakFlag && vakuumMotorRunningFlag) { // Hvis strømmen for vakuum peaker
	  vCurrentPeakFlag = 1;
  }
  if(!vakuumMotorRunningFlag) { // Hvis ikke motoren er på, skal peaken resettes
	  vCurrentPeakFlag = 0;
  }
  if(abs(vCurrent) < 1.30 && vCurrentPeakFlag) { // Hvis strømmen har falt etter den har peaket.
	  vakuumDoneFlag = 1;
  }

  Serial.println(vCurrent);
  delay(10); //for debouncing
}
