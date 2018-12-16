#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>
#include <VL53L0X.h>
#include "Hbro.h"
#include "CurrentSensor30A.h"
#include "common.hpp"
#include "Fulness.hpp"

int pumpPWMPin = 10; 	//bestemmer forrovehastigheten på pumpe motoren
int lukeForPWMPin = 9; 	//gir forrover signal til H-boren, bakover PWM må være 0 for at den skal fungere
int lukeBackPWMPin = 6;  //gir bakover signal til H-broen, forrover PWM må være 0 for at den skal fungere

int pirSensorPin = 16;
int movementLedPin = 14;
int pumpButtonPin  = 8; //kanpp for å starte pumpen, når den er inne kjører pumpen
int lidOpenButtonPin = 7;
int lidCloseButtonPin = 4;
int highPin = 5; //bare for å ta ut 5V TODO: Check if this is needed, can't we just directly wire to Vcc?
int currentSensorPin = A0;
float current = 0;
int openFlag = 0; //er 1 når lokket er oppe og 0 når det er alt annet
int fulnessDistance = 1000; //brukes for å lagre fulness fra forrige loop

int vacuumCurrentSensorPin = A3;
float vCurrent = 0;
int vCurrentPeakFlag = 0;
int vacuumDoneFlag = 0;
int vacuumMotorRunningFlag = 0;

VL53L0X distanceSensor; // Object for distance measurement

unsigned long timeVacuumStart = 0;
int vacuumTimingStarted = 0;

Hbro vacuumMotor(pumpPWMPin , 0, 0, 0); //setter opp et objekt for pumpen
Hbro lukeMotorer(lukeForPWMPin, lukeBackPWMPin, 0, 0); //setter opp et objekt for luken, her blir begge motorene kjørt i paralell
CurrentSensor30A lukeCurrent(currentSensorPin);
CurrentSensor30A vacuumCurrent(vacuumCurrentSensorPin);

// New state machine variables
FullnessLevel fullnessLevel;
LidState lidState;
VacuumState vacuumState;
ButtonMode buttonMode;

// Lid top or bottom sensors and values
int lidTopSensor = A1;
int lidBottomSensor = 15;

unsigned long lastMovementTime; // Time when last movement was recorded
unsigned long lastCompressTime; // Time when the last compression occured
unsigned long longCompressTimeInterval_ms = 10000; // 10 seconds
unsigned long shortCompressTImeInterval_ms = 5000;	// 5 seconds

int usualForceThreshold = 0;	// if the lid force is over this value, then something is in the way
int fullnessThreshold = 150;	// If the distance to the bin is below this threshold, the bin is full

/* 
  /$$$$$$              /$$                        
 /$$__  $$            | $$                        
| $$  \__/  /$$$$$$  /$$$$$$   /$$   /$$  /$$$$$$ 
|  $$$$$$  /$$__  $$|_  $$_/  | $$  | $$ /$$__  $$
 \____  $$| $$$$$$$$  | $$    | $$  | $$| $$  \ $$
 /$$  \ $$| $$_____/  | $$ /$$| $$  | $$| $$  | $$
|  $$$$$$/|  $$$$$$$  |  $$$$/|  $$$$$$/| $$$$$$$/
 \______/  \_______/   \___/   \______/ | $$____/ 
                                        | $$      
                                        | $$      
                                        |__/      
 */
void setup() {
	Serial.begin(9600);
  	vacuumMotor.setupFuction(); //setter inputs og outputs
	Serial.println("Vacuum motor setup complete");
  	// vacuumMotor.enableDebugging(); //skriver info til konsoll så man kan overvåke
  	lukeMotorer.setupFuction(); //setter inputs og outputs
	Serial.println("Luke motor setup complete");
  	// lukeMotorer.enableDebugging(); //skriver info til konsoll så man kan overvåke

  	pinMode(pumpButtonPin, INPUT); //tar inn signal fra knapp. 5V er høy 0V er lav
  	pinMode(lidOpenButtonPin, INPUT); //tar inn signal fra knapp. 5V er høy 0V er lav
  	pinMode(lidCloseButtonPin, INPUT); //tar inn signal fra knapp. 5V er høy 0V er lav
  	pinMode(highPin, OUTPUT); //pinne for 5V
    pinMode(lidTopSensor, INPUT);
    pinMode(lidBottomSensor, INPUT);
	pinMode(pirSensorPin, INPUT);
	pinMode(movementLedPin, OUTPUT);
  	digitalWrite(highPin, HIGH); //pinne for 5V

	Serial.println("Inputs and outputs setup complete");

  	Wire.begin(); // Needed for fullness sensor
  	distanceSensor.init();	// Initiating fullness sensor
	distanceSensor.setTimeout(500);	// Timeout for reading a fullness sensor value
	distanceSensor.startContinuous();
 
	Serial.println("Fulness sensor setup complete");

	startupRoutine();

	Serial.println("Startup routine complete");

}

/*                     
 /$$      /$$           /$$                 /$$                                    
| $$$    /$$$          |__/                | $$                                    
| $$$$  /$$$$  /$$$$$$  /$$ /$$$$$$$       | $$        /$$$$$$   /$$$$$$   /$$$$$$ 
| $$ $$/$$ $$ |____  $$| $$| $$__  $$      | $$       /$$__  $$ /$$__  $$ /$$__  $$
| $$  $$$| $$  /$$$$$$$| $$| $$  \ $$      | $$      | $$  \ $$| $$  \ $$| $$  \ $$
| $$\  $ | $$ /$$__  $$| $$| $$  | $$      | $$      | $$  | $$| $$  | $$| $$  | $$
| $$ \/  | $$|  $$$$$$$| $$| $$  | $$      | $$$$$$$$|  $$$$$$/|  $$$$$$/| $$$$$$$/
|__/     |__/ \_______/|__/|__/  |__/      |________/ \______/  \______/ | $$____/ 
                                                                         | $$      
                                                                         | $$      
                                                                         |__/      
 */
void loop() {
	// Button Override Routine
	int buttonOverrideResult = buttonOverride();

	if (buttonOverrideResult == 2) {
		//TODO: This section  within the if isn't properly implemented, need to test
		int movement = digitalRead(pirSensorPin);
		uint16_t fullness = readFulnessLevel();

		if (movement) {
			Serial.print("movement ");
			digitalWrite(movementLedPin,HIGH);
			movementRoutine();
			vacuumState = Off;
			lidState = Open; //open
		}
		else {
			digitalWrite(movementLedPin,LOW);
			unsigned long lastMovementTimeInterval = millis() - lastMovementTime;

			if (lastMovementTimeInterval > longCompressTimeInterval_ms) {
				Serial.print("no movement routine ");
				noMovementRoutine();
			}
			else {
				Serial.print("no movement too short time intervell ");
				vacuumState = Off;
				lidState = Open; //open
			}

		}

	}
	else { digitalWrite(movementLedPin,LOW);}

	lidStateRoutine();

	pumpStateRoutine();


	//displayFullness(); // Distance measurement messes up the lid timing, shouldn't be an issue in the real system, because it will only measure when lidAtTop is true

	Serial.println();

	delay(10);
}
