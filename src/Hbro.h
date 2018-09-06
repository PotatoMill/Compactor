#ifndef ds
#define ds

#include <Arduino.h>

class Hbro{
public:
  //constructors
  Hbro(); //default constructor
  Hbro(int forPWMPin, int backPWMPin, int forEnablePin, int backEnablePin); //constructor setting up pins

  void setSpeed(bool direction, int speed); //1 is forwards 0 is bakwards. Speed range from 0 to 255. Ramping speed,
  //nb! top speed might not work
  void smoothStop(); //ramp stopping 5 times faster than the ramp time
  void stop(); //sudden stop, can be bad for the system
  void setupFuction();

  void setForPWMPin(int pinNumber);
  void setBackPWMPin(int pinNumber);
  void setForEnablePin(int pinNumber);
  void setBackEnablePin(int pinNumber);
  void setRampTime(int newRampTime);

  int getForPWMPin();
  int getBackPWMPin();
  int getForEnablePin();
  int getBackEnablePin();

  void enableDebugging();
  void disableDebugging();

private:
  void speedSetter(); //setter hastighet og passer på at revers også har motsatt PWM verdier

  int forPWMPin; //forwards PWM pin
  int backPWMPin; //backwards PWM pin
  int activePWMPin; //the PWM pin currently setting the speed and direction
  int forEnablePin; //forwards enable pin. Not nessesary on all boards
  int backEnablePin; //backwards enable pin. Not nessesary on all boards
  int speed; //speed ranging from 0 to 255 where 255 is max speed
  bool direction; //1 is forwards 0 is backwards
  int rampTime; //time to ramp to set speed
  bool debugging; //1 to enable debugging
};

#endif
