#pragma once

/*
	Because it is a -30 to +30 Module, the resolution is 66mV per ampere on the output pin.
*/


class PowerMeter30A {
private:
	double m_current{0}; // Current
	double m_voltage{0}; // Voltage
	int m_rawValue{0}; // Value used for reading sensor value
	int m_mVPerAmp{66}; // Scale factor
	int m_acOffset{2490}; // The AC-offset
	int m_powerMeterPin; // Pin to read

public:

	PowerMeter30A(); //Default constructor
	PowerMeter30A(int analogSignalPin); // Constructor, where the input signal pin can be selected

	~PowerMeter30A(); // Destructor

	double getCurrent(); // Returns Current in Amps
	void setAnalogSignalPin(int pinNumber); // Setting the pin for the input signal
};
