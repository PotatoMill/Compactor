#include <Arduino.h>
#include "PowerMeter30A.h"


PowerMeter30A::PowerMeter30A(){} // Default constructor

PowerMeter30A::PowerMeter30A(int analogSignalPin): m_powerMeterPin(analogSignalPin) {} //Constuctor

PowerMeter30A::~PowerMeter30A() {} // Default Destructor, no need to do anything

double PowerMeter30A::getCurrent() {
	m_rawValue = analogRead(m_powerMeterPin);
	m_voltage = (m_rawValue/1024.0) * 5000;
	m_current = ((m_voltage - m_acOffset) /m_mVPerAmp);
	return m_current;
}

void PowerMeter30A::setAnalogSignalPin(int pinNumber){
	m_powerMeterPin = pinNumber;
}
