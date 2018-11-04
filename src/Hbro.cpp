#include <Arduino.h>
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
	oldTime{micros()},
	timeInterval{0},
    rampTime(5){}

void Hbro::delaySetSpeed(bool newDirection, int newSpeed){
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
    //hvis den nye hastighetnen er mindre må hastigheten økes
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

void Hbro::setSpeed(bool newDirection, int newSpeed){
    //hvis det er feil retning må den bremse ned først, sifte retning og øke opp til ønsket hastighet
	if(newDirection != direction || changeDirMode){ // Systemet vil gå i denne "if"en hvis retning er byttet, helt til speed = 0
	  changeDirMode = 1;
	  unsigned long newTime = micros();
	  timeInterval = newTime - oldTime;
	  if(timeInterval < 1000 && timeInterval > 0) { // Vent til timeInterval har blitt større
	  }
	  else if(timeInterval >= 1000 && timeInterval < 20000) {
		  	int speedChange = timeInterval / 1000; // new change in speed
			if(speedChange > 8) { // If the change in speed is too big, now set to 8
				speedChange = 8;
			}
			speed -= speedChange;
			if(speed < 0) { // If the speed in speed change mode have fallen below 0 due to speed decrements
				speed = 0;
			}
	  		oldTime = newTime;
	  }
	  else if(timeInterval <= 0) {	// Sett oldTime = newTime, men ikke juster speed. Dette er edge case når variabelen overflower
	  	oldTime = newTime;
	  }
	  else if(timeInterval > 20000) { // If something have happened so that the time interval is too big
	  	oldTime = newTime;
	  }
	  speedSetter();
	  if(speed <= 0) { // Speed er 0, og vi kan gå ut av changeDirMode
		speed = 0;	// Reduntant set to 0 if we are 0 or below
		speedSetter();
		changeDirMode = 0; // Can now exit changeDirMode
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
   }
	// Hvis speeden allerede er satt, kan vi bare gå ut med en gang. Andre del etter else kan nok fjernes, men er et ekstra sikkerhetsnett. bytt ut 250 med max speed.
	// Kanskje legge en member variable til istedenfor 250.
	if(newSpeed == this->speed || (newSpeed == 250 && this->speed>= 250)) {
		// Serial.println("In return statement");
		return;
	}
    if(newSpeed < speed && !changeDirMode){ // Hvis den nye speeden er mindre enn current speed, må vi redusere current speed
        if(debugging){Serial.println(speed);};

		unsigned long newTime = micros();
		timeInterval = newTime - oldTime;

		if(timeInterval < 1000 && timeInterval > 0) { // Vent til timeInterval har blitt større
		}
		else if(timeInterval >= 1000 && timeInterval < 20000) {
			int speedChange = timeInterval / 1000;
			if(speedChange > 8) {
				speedChange = 8;
			}
			speed -= speedChange;
			if(speed < newSpeed) { // Hvis vi minsker fart, og speed faller under newSpeed, så set speed til newSpeed
				speed = newSpeed;
			}
			oldTime = newTime;
		}
		else if(timeInterval < 0) {	// Sett oldTime == newTime, men ikke juster speed. Dette er edge case når variabelen overflower
			oldTime = newTime;
		}
		else if(timeInterval > 20000) {
			oldTime = newTime;
		}
        speedSetter();
		// Serial.println(speed);
    }
    else{ // Hvis den nye speeden er større enn current speed, må vi øke current speed
		if(changeDirMode) { // Hvis vi er i change dir mode returner vi
			return;
		}
            if(debugging){Serial.println(speed);};

			unsigned long newTime = micros();
			timeInterval = newTime - oldTime;
			if(timeInterval < 1000 && timeInterval > 0) { // Vent til timeInterval har blitt større
			}
			else if(timeInterval >= 1000 && timeInterval < 20000) {
				int speedChange = timeInterval / 1000;
				if (speedChange > 8) {
					speedChange = 8;
				}
				speed += speedChange;
				if (speed > newSpeed) { // Hvis vi øker farten, og speed går over newSpeed, set speed = newSpeed
					speed = newSpeed;
				}
				oldTime = newTime;
			}
			else if(timeInterval < 0) {	// Sett oldTime == newTime, men ikke juster speed. Dette er edge case når variabelen overflower
				oldTime = newTime;
			}
			else if(timeInterval > 20000) {
				oldTime = newTime;
			}
			speedSetter();
			// Serial.println(speed);
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

// Below is for fixing delay
int Hbro::getSpeed() const {
	return this->speed;
}
