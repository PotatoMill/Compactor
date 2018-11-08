#pragma once

#include "Arduino.h"

class UltraSonic {
private:
	int trigPin;
	int echoPin;
	long duration;
	long distanceCM;
public:
	UltraSonic(int triggerPin, int echoPin);
	void timing();
	long getObjectDistance();
	long getFilteredObjectDistance(int numFilterElem);
};
