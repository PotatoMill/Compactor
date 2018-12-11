#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>
#include <VL53L0X.h>
#include "Hbro.h"
#include "CurrentSensor30A.h"
#include "common.hpp"

int pumpPWMPin = 10; 	//bestemmer forrovehastigheten på pumpe motoren
int pumpEnablePin = 16; //enabler forover. Kan være høy hele siden PWM styrer hastighet. Høy er på
int lukeForPWMPin = 9; 	//gir forrover signal til H-boren, bakover PWM må være 0 for at den skal fungere
int lukeBackPWMPin =6;  //gir bakover signal til H-broen, forrover PWM må være 0 for at den skal fungere

int pirSensorPin; // TODO: Find out which pin to use
int pumpButtonPin  = 8; //kanpp for å starte pumpen, når den er inne kjører pumpen
int lidOpenButtonPin = 7;
int lidCloseButtonPin = 4;
int highPin = 5; //bare for å ta ut 5V TODO: Check if this is needed, can't we just directly wire to Vcc?
int currentSensorPin = A0;
float current = 0;
int openFlag = 0; //er 1 når lokket er oppe og 0 når det er alt annet


int vacuumCurrentSensorPin = A3;
float vCurrent = 0;
int vCurrentPeakFlag = 0;
int vacuumDoneFlag = 0;
int vacuumMotorRunningFlag = 0;

VL53L0X distanceSensor; // Object for distance measurement

unsigned long timeVacuumStart = 0;
int vacuumTimingStarted = 0;

Hbro vacuumMotor(pumpPWMPin , 0, pumpEnablePin, 14); //setter opp et objekt for pumpen
Hbro lukeMotorer(lukeForPWMPin, lukeBackPWMPin, 0, 0); //setter opp et objekt for luken, her blir begge motorene kjørt i paralell
CurrentSensor30A lukeCurrent(currentSensorPin);
CurrentSensor30A vacuumCurrent(vacuumCurrentSensorPin);

// New state machine variables
FullnessLevel fullnessLevel;
LidState lidState;
VacuumState vacuumState;
ButtonMode buttonMode;

// Lid top or bottom sensors and values
int lidTopSensor; 	// TODO: Figure out pin for this sensor
int lidBottomSensor;	// TODO: Figure out pin for this sensor
int lidAtTop = 0;		// Not sure if we need this variable
int lidAtBotton = 0;	// Not sure if we need this variable

unsigned long lastMovementTime; // Time when last movement was recorded
unsigned long lastCompressTime; // Time when the last compression occured
unsigned long longCompressTimeInterval_ms = 300000; // 5 minutes
unsigned long shortCompressTImeInterval_ms = 60000;	// 1 minute

int usualForceThreshold = 0;	// if the lid force is over this value, then something is in the way
uint16_t fullnessThreshold = 150;	// If the distance to the bin is below this threshold, the bin is full


void setup() {
	Serial.begin(9600);
  	vacuumMotor.setupFuction(); //setter inputs og outputs
  	// vacuumMotor.enableDebugging(); //skriver info til konsoll så man kan overvåke
  	lukeMotorer.setupFuction(); //setter inputs og outputs
  	// lukeMotorer.enableDebugging(); //skriver info til konsoll så man kan overvåke

  	pinMode(pumpButtonPin, INPUT); //tar inn signal fra knapp. 5V er høy 0V er lav
  	pinMode(lidOpenButtonPin, INPUT); //tar inn signal fra knapp. 5V er høy 0V er lav
  	pinMode(lidCloseButtonPin, INPUT); //tar inn signal fra knapp. 5V er høy 0V er lav
  	pinMode(highPin, OUTPUT); //pinne for 5V
  	digitalWrite(highPin, HIGH); //pinne for 5V

  	Wire.begin(); // Needed for fullness sensor
  	distanceSensor.init();	// Initiating fullness sensor
	distanceSensor.setTimeout(500);	// Timeout for reading a fullness sensor value
	distanceSensor.startContinuous();


	startupRoutine();

}


void loop() {
	// Button Override Routine
	int buttonOverrideResult = buttonOverride();

	if (buttonOverrideResult == 2) {
		//TODO: This section  within the if isn't properly implemented, need to test when writing
		int movement = checkForMovement(1);
		if (movement) {
			movementRoutine();
			vacuumState = Off;
			lidState = Open;
		}
		else {
			unsigned long lastMovementTimeInterval = millis() - lastMovementTime;
			if (lastMovementTimeInterval > longCompressTimeInterval_ms) {
				noMovementRoutine();
			}
			else {
				vacuumState = Off;
				lidState = Open;
			}
		}
	}

	// Lid State Routine
	lidStateRoutine();

	// Pump State Routine
	pumpStateRoutine();

	displayFullness();


	delay(10);
}
