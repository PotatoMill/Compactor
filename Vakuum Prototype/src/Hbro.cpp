#include "Hbro.h"

Hbro::Hbro(){} //default constructor
Hbro::Hbro(int forPWMPin, int backPWMPin, int forEnablePin, int backEnablePin):
    forPWMPin(forPWMPin),
    backPWMPin(backPWMPin),
    activePWMPin(forPWMPin),
    forEnablePin(forEnablePin),
    backEnablePin(backEnablePin),
    speed(0),
    direction(1),
    rampTime(5){}

void Hbro::setSpeed(bool newDirection, int newSpeed){
    //hvis det er feil retning må den bremse ned først, sifte retning og øke opp til ønsket hastighet
    if(newDirection != direction){
      //reduserer hastigheten til 0
      while(speed > 0){
          if(debugging){Serial.println(speed);};
          speed -= 4;
          speedSetter(); //setter hastighet
          delay(rampTime);
      }
      //skifter retning og activePWMPin
      direction = newDirection;
      if(direction == 1){activePWMPin = forPWMPin;}
      if(direction == 0){activePWMPin = backPWMPin;}
      if(debugging){
        Serial.println("Direction is now:");
        Serial.println(direction);
        Serial.println("ActivePWMpin is now:");
        Serial.println(activePWMPin);
      }
    }

    //skifter hastigheten fra nåværende til ny hastighet
    //hvis den nye hastighetnen er mindre må
    if(newSpeed < speed){
        while(newSpeed < speed ){
            if(debugging){Serial.println(speed);};
            speed -= 4;
            speedSetter();
            delay(rampTime);
        }
    }
    //hvis hastigheten er større må den minskes
    else{
        while(newSpeed > speed){
            if(debugging){Serial.println(speed);};
            speed += 4;
            speedSetter();
            delay(rampTime);
        }
    }
}

void Hbro::smoothStop(){
    if(debugging){Serial.println("Smooth Stop");};
    while(speed > 0){
      speed -= 4;
      speedSetter();
      delay(rampTime/5);
    }
}

void Hbro::stop(){
    if(debugging){Serial.println("Stop");};
    speed = 0;
    //setter begge PWM pinnen til 0 for sikkerhets skyld
    analogWrite(forPWMPin, 0);
    analogWrite(backPWMPin, 0);
}

void Hbro::setupFuction(){
    if(forPWMPin!=0){
      pinMode(forPWMPin, OUTPUT);
      analogWrite(forPWMPin, 0);
    }
    if(backPWMPin!=0){
      pinMode(backPWMPin, OUTPUT);
      analogWrite(backPWMPin, 0); //0 for revers er 255
    }
    if(forEnablePin!=0){
      pinMode(forEnablePin, OUTPUT);
      digitalWrite(forEnablePin, HIGH);
    }
    if(backEnablePin!=0){
      pinMode(backEnablePin, OUTPUT);
      digitalWrite(backEnablePin, HIGH);
    }


     //skrur på enable med en gang så er det bare PWM som kontrolerer

    if(debugging){Serial.println("setup finished");};
}

void Hbro::setForPWMPin(int pinNumber){forPWMPin=pinNumber;}
void Hbro::setBackPWMPin(int pinNumber){backPWMPin=pinNumber;}
void Hbro::setForEnablePin(int pinNumber){forEnablePin=pinNumber;}
void Hbro::setBackEnablePin(int pinNumber){backEnablePin=pinNumber;}
void Hbro::setRampTime(int newRampTime){rampTime=newRampTime;}

int Hbro::getForPWMPin(){return forPWMPin;}
int Hbro::getBackPWMPin(){return backPWMPin;}
int Hbro::getForEnablePin(){return forEnablePin;}
int Hbro::getBackEnablePin(){return backEnablePin;}

void Hbro::enableDebugging(){
    debugging = true;
    if(debugging){Serial.println("debugging enabeled");};
}
void Hbro::disableDebugging(){debugging = false;}

void Hbro::speedSetter(){
  if(activePWMPin == 0 && debugging == true){Serial.println("Error: activePWMPin is zero");}
  else{analogWrite(activePWMPin, speed);}
}
