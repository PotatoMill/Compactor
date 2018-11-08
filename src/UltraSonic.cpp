#include "Arduino.h"
#include "UltraSonic.hpp"
#include "Wire.h"

UltraSonic::UltraSonic(int trigPin, int echoPin)
{
	pinMode(trigPin,OUTPUT);
   	pinMode(echoPin,INPUT);
	this->trigPin = trigPin;
	this->echoPin = echoPin;
}

void UltraSonic::timing()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin,HIGH);
}

long UltraSonic::getObjectDistance() {
  timing();
  distanceCM = (duration /29) / 2; // Because the ultrasonic wave travels at 330m/s, we need to devide by 29 to get us/cm //TODO: Verify this
  return distanceCM;

}

long UltraSonic::getFilteredObjectDistance(int numFilterElem) { // Reading sensor "numFilterElem" times, and placing them in sorted order into an array. Returning median
	long lastReadings[numFilterElem]; // Array for storing values
	lastReadings[0] = getObjectDistance(); // Reading sensor
	int med = numFilterElem / 2; // Finding the median element
	int i, key, j;
	for(i = 1; i < numFilterElem; i++) {
		delay(1);
		lastReadings[i] = getObjectDistance(); // Reading sensor
		// Sorting value into correct position
		key = lastReadings[i];
		j = i-1;
		while (j >= 0 && lastReadings[j] > key) {
			lastReadings[j+1] = lastReadings[j];
			j = j-1;
		}
		lastReadings[j+1] = key;
	}

	Serial.print("\n");
	return lastReadings[med]; // Returning the median element
}
