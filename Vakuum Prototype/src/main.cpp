#include <Arduino.h>
#include "Hbro.h"

int pumpPWMPin = 10; //10
int pumpEnablePin = 16;
int lLidForPWMPin = 9; //9
int lLidBackPWMPin = 6;
int rLidForPWMPin = 5;
int rLidBackPWMPin = 3;

Hbro vakuumMotor(pumpPWMPin , 0, pumpEnablePin, 14);
Hbro rightMotor(rLidForPWMPin, rLidBackPWMPin, 0, 0);
Hbro leftMotor(lLidForPWMPin, lLidBackPWMPin, 0, 0);

void setup() {
  Serial.begin(9600);
  vakuumMotor.setupFuction();
  rightMotor.setupFuction();
  leftMotor.setupFuction();
  vakuumMotor.enableDebugging();
  vakuumMotor.setRampTime(5);
  leftMotor.setRampTime(10);
  rightMotor.setRampTime(10);
}



void loop() {
    vakuumMotor.setSpeed(1,250);
    delay(1000);
    vakuumMotor.setSpeed(1,0);
    rightMotor.setSpeed(1,250);
    rightMotor.setSpeed(0,250);
    delay(1000);
    rightMotor.setSpeed(1,0);
    leftMotor.setSpeed(1,250);
    leftMotor.setSpeed(0,250);
    delay(1000);
    leftMotor.setSpeed(1,0);

}
