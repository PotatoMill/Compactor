#include <Arduino.h>
#include "CurrentSensor30A.h"


CurrentSensor30A::CurrentSensor30A(){} // Default constructor

CurrentSensor30A::CurrentSensor30A(int analogSignalPin): m_powerMeterPin(analogSignalPin) {} //Constuctor

CurrentSensor30A::~CurrentSensor30A() {} // Default Destructor, no need to do anything

double CurrentSensor30A::getCurrent() {
	m_rawValue = analogRead(m_powerMeterPin);
	m_voltage = (m_rawValue/1024.0) * 5000;
	m_current = ((m_voltage - m_acOffset) /m_mVPerAmp);
	return m_current;
}

void CurrentSensor30A::setAnalogSignalPin(int pinNumber){
	m_powerMeterPin = pinNumber;
}
