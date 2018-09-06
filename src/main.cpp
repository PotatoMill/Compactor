#include <Arduino.h>
#include "Hbro.h"

int pumpPWMPin = 10; //10
int pumpEnablePin = 16;

Hbro vakuumMotor(pumpPWMPin , 0, pumpEnablePin, 14);

void setup() {
  Serial.begin(9600);
  vakuumMotor.setupFuction();
  vakuumMotor.enableDebugging();

}



void loop() {
    vakuumMotor.setSpeed(1,250);
    delay(1000);
    vakuumMotor.setSpeed(1,0);

}
