#include <Arduino.h>
#include <Servo.h>
#include "Hbro.h"

int pumpPWMPin = 3; //10
int pumpEnablePin = 16;

int pumpButtonPin  = 8;
int lidButtonPin = 7;
int stopButtonPin = 6;
int highPin = 5;

int servoPin = 9;
int servoState = 180; //vinkel servoen står på i grader. Range 0-180

Servo lukeServo; //setter opp en klasse for  servoen til luken
Hbro vakuumMotor(pumpPWMPin , 0, pumpEnablePin, 14); //setter opp en klasse for vakuum motoren

void setup() {
  Serial.begin(9600);
  vakuumMotor.setupFuction();
  vakuumMotor.enableDebugging();

  pinMode(pumpButtonPin, INPUT);
  pinMode(lidButtonPin, INPUT);
  pinMode(stopButtonPin, INPUT);
  pinMode(highPin, OUTPUT);
  digitalWrite(highPin, HIGH);

  lukeServo.attach(servoPin);
}


void loop() {

  if(digitalRead(pumpButtonPin) == HIGH){ //hvis knappen for pumpen er inne skal den på
    vakuumMotor.setSpeed(1,250);
    Serial.println("Pump on");
  }
  else{
    vakuumMotor.setSpeed(1,0); //hvis knappen for pumpen er ute skal den av
    Serial.println("Pump off");
  }

  if(digitalRead(lidButtonPin)==HIGH){ //hvis luke knappen blir trykket
    if(servoState == 180){ //hvis den allerede er åpen må den lukkes
      lukeServo.write(0);
      delay(1000); //venter litt så hvis knappen blir hold inn så vil den ikke skifte retning med en gang
      servoState = 0;
      Serial.println("Change servo to 0");
    }
    else{
      lukeServo.write(180);
      delay(1000); //venter litt så hvis knappen blir hold inn så vil den ikke skifte retning med en gang
      servoState = 180;
      Serial.println("Change servo to 180");
    }
  }
  delay(200);

}
