#include "common.hpp"

/*
 A file containing the blocks in the state diagram as functions. Thought this would be a good way to start
*/


/*
* startupRoutine() - The first function that is run on the system.
*/
void startupRoutine() {
	fullnessLevel = NotFull;
	lastMovementTime = millis();
	lastCompressTime = longCompressTimeInterval_ms;
	vacuumState = Off;
	lidState = Stop;
	buttonMode = Manual; // TODO: Put into manual for now, is this correct?
}

/*
* buttonOverride() - A function that checks the button mode, either automatic or manual.
*					Manual mode acts according to what button is pressed at the time.
*					Automatic enters a "mode" based on what button is pressed once, and acts out the command.
*
* Return: TODO: Not completely sure of the exact reason why it should return 1 or 2.
* TODO: Implement automatic section
*/
int buttonOverride() {
	// TODO: Figure out what should indicate if it is in manual mode, right now, if any button is pressed, it goes into manual mode.'
	// TODO: Find out if the pump should be turned off when exiting in the last else

	if (buttonMode == Manual) {
		if (digitalRead(lidOpenButtonPin) && digitalRead(lidCloseButtonPin)) { // If both the buttons are pressed
			if(digitalRead(pumpButtonPin))
				vacuumState = On;
			else
				vacuumState = Off;
			return 1;
		}
		else if (digitalRead(lidOpenButtonPin)) { // If the lid open button is pressed
			lidState = Open;
			if (digitalRead(pumpButtonPin))
				vacuumState = On;
			else
				vacuumState = Off;
			return 1;
		}
		else if (digitalRead(lidCloseButtonPin)) { // If the lid close button is pressed
			lidState = Close;
			if (digitalRead(pumpButtonPin))
				vacuumState = On;
			else
				vacuumState = Off;
		}
		else {	// When none of the buttons are pressed
			if (digitalRead(pumpButtonPin)) {
				lidState = Stop;
				vacuumState = On;
				return 1;
			}
			else {
				lidState = Stop; // Think there is an error in the state diagram, becasue the lidState should not be changed if diagram is followed strictly
				return 2;
			}
		}
	}
	else {
		//TODO: Implement automatic version
	}

	return 1;
}

/*
* lidStateRoutine() - Function that checks the lid state, and issues command to the motor. It can also change the state, e.g. if lidTopSensor is true.
*/
void lidStateRoutine() {
	if (lidState == Stop) {
		lukeMotorer.setSpeed(0, 0);
	}
	else if (lidState == Open) {
		if (1 == 0) { // if (digitalRead(lidTopSensor)) { TODO: Replace the first if with the second when top sensor implemented
			lidState = Stop;
			lukeMotorer.setSpeed(0, 0);
		}
		else {
			if (current < usualForceThreshold) { // TODO: Find out how to know if usualForceThreshold is reached
				lidState = Stop;
				lukeMotorer.setSpeed(0, 0);
			}
			else {
				lukeMotorer.setSpeed(0, 150);
			}
		}
	}
	else if (lidState == Close) {
		if (lidAtBotton) {
			lukeMotorer.setSpeed(1, 250);
		}
		else {
			if (current < usualForceThreshold) { // TODO: Find out how to know if usualForceThreshold is reached
				lidState = Stop;
				lukeMotorer.setSpeed(0, 0);
			}
			else {
				lukeMotorer.setSpeed(1, 150);
			}
		}
	}
}

/*
* pumpStateRoutine() - Function that checks the vacuum pump state, and issues command to the motor.
* TODO: Find out how to signal vacuumDoneFlag
*/
void pumpStateRoutine() {
	if (vacuumState == Off) {
		vacuumMotor.setSpeed(1, 0);
	}
	else {
		if (digitalRead(lidBottomSensor)) {
			if (vacuumDoneFlag) {
				lidState = Open;
				vacuumState = Off;
				lastCompressTime = millis();
				vacuumMotor.setSpeed(1, 0);
			}
			else {
				vacuumState = On;
				vacuumMotor.setSpeed(1, 250);
			}
		}
	}
}


void movementRoutine() {
	lastMovementTime = millis();
	if (vacuumState == On) {
		//TODO: Save to some kind of log that the vacuum process was interrupted
	}
}

void noMovementRoutine() {
	uint16_t distanceFromSensor = readFullnessLevel(1);
	if (distanceFromSensor < fullnessThreshold) { // If the bin is full
		//TODO: Implement difference between fullAndCantCompressMore and fullNeedsCompress
		//TODO: Implement automatic compression
	}
	else {
		vacuumState = Off;
		lidState = Open;
	}
}

/*
* readFullnessLevel() - Function for reading the fullness level
*
* @arg1: How many readings to do in order to find a correct sensor value
* Return: The filtered sensor value
*/
uint16_t readFullnessLevel(int numReadings) {
	uint16_t sensorValue = distanceSensor.readRangeContinuousMillimeters();
	for (int i = 0; i < numReadings; i++) {
		sensorValue = 0.9 * sensorValue + 0.1 * distanceSensor.readRangeContinuousMillimeters();
	}
	return sensorValue;
}

/*
* displayFullness() - Function that checks the bin fullness, and saves the state. The fullness is logged every 10'th minute
*/
void displayFullness() {
	if (digitalRead(lidTopSensor)) { // Checks if the lid is open, if not then it exits immediately
		uint16_t distanceFromSensor = readFullnessLevel(5);
		if (distanceFromSensor > 5000) {
			//TODO: Sensor value too high, do something because of this?
		}
		//TODO: fulnessIndicator = distanceFromSensor; // In some kind of way
		//TODO: About the logging, are we going to use a SD card or something?
	}
}

/*
* checkForMovement() - Function that reads the pir sensor(s) for movement
* @arg1: Time inteval the system should look for movement (in ms).
* Return: 1 for movement, 0 for no movement
*/
int checkForMovement(unsigned long timeIntervalms) {
	unsigned long movementSenseDuration = 0;
	unsigned long startMovementSens = millis();
	while (timeIntervalms > movementSenseDuration) {
		if (digitalRead(pirSensorPin)) { // Checks if movement is detected
			return 1;
		}
		movementSenseDuration = millis() - startMovementSens;
	}
	return 0;
}
