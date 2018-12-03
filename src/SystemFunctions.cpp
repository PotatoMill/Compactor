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
	Serial.println("In buttonOverride");

	if (buttonMode == Manual) {
		if (digitalRead(lidOpenButtonPin && lidCloseButtonPin)) { // If both the buttons are pressed
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
				lidState = Stop; // Think there is an error in the state diagram, if the diagram is to be strictly followed, then this should not happen
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
		if (analogRead(lidTopSensor)) {
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

void pumpStateRoutine() {
	if (vacuumState == Off) {
		vacuumMotor.setSpeed(1, 0);
	}
	else {
		if (analogRead(lidBottomSensor)) {
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
