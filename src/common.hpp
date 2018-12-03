#pragma once
#include <Arduino.h>
#include <Servo.h>
#include "UltraSonic.hpp"
#include "Hbro.h"
#include "CurrentSensor30A.h"
#include "VL53L0X.h"

/*
	This file contains:
	- Global variables and objects used in different files, in order to separate the system into different functions.
	- Definitions for states used throughout the system
	- Function declerations for system functions
*/

// Enum for fullness level
enum FullnessLevel {NotFull, CantCompressMore, NeedsCompress};

// Enum for lid state
enum LidState {Stop, Close, Open};

// Enum for vacuum motor state
enum VacuumState {Off, On};

// Enum for buttonMode
enum ButtonMode {Manual, Automatic};

// Extern variables, declared in main.cpp
extern int pumpPWMPin; //bestemmer forrovehastigheten på pumpe motoren
extern int pumpEnablePin; //enabler forover. Kan være høy hele siden PWM styrer hastighet. Høy er på
extern int lukeForPWMPin; //gir forrover signal til H-boren, bakover PWM må være 0 for at den skal fungere
extern int lukeBackPWMPin; //gir bakover signal til H-broen, forrover PWM må være 0 for at den skal fungere

extern int pumpButtonPin; //kanpp for å starte pumpen, når den er inne kjører pumpen
extern int lidOpenButtonPin;
extern int lidCloseButtonPin;
extern int highPin; //bare for å ta ut 5V
extern int currentSensorPin;
extern float current;
extern int openFlag; //er 1 når lokket er oppe og 0 når det er alt annet

extern int vacuumCurrentSensorPin;
extern float vCurrent;
extern int vCurrentPeakFlag;
extern int vacuumDoneFlag;
extern int vacuumMotorRunningFlag;

extern unsigned long timeVacuumStart;
extern int vacuumTimingStarted;


// Extern variables for the new state machine, defined in main.cpp
extern FullnessLevel fullnessLevel; 				// Current fullness level of the bin
extern LidState lidState;							// Current lid state
extern VacuumState vacuumState;						// Current vacuum motor state

extern ButtonMode buttonMode;

extern unsigned long lastMovementTime;				// Time since last detected movement
extern unsigned long lastCompressTime;				// Time since last compression
extern unsigned long longCompressTimeInterval_ms;	// Long time interval for compression
extern unsigned long shortCompressTImeInterval_ms;	// Short time interval for compression


// Lid top or bottom sensors
extern int lidTopSensor;
extern int lidBottomSensor;
extern int lidAtTop;
extern int lidAtBotton;
extern int usualForceThreshold; // Set to some value that represents if some force threshold is reached


// Extern objects, defined in main.cpp
extern Hbro vacuumMotor; //setter opp et objekt for pumpen
extern Hbro lukeMotorer; //setter opp et objekt for luken, her blir begge motorene kjørt i paralell
extern CurrentSensor30A lukeCurrent;
extern CurrentSensor30A vacuumCurrent;

// Extern functions, implementation in SystemFunctions.cpp
void startupRoutine(); 	// The systems startup routine
int buttonOverride();
void lidStateRoutine();
void pumpStateRoutine();
