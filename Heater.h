#ifndef Heater_h
#define Heater_h
#include "Arduino.h"
#include "MAX31865.h"

class Heater
{
public:
	Heater();
	Heater(int _PWMPin);
	void initialHeatingTime(double heatCapacity, double mass, double currTemp, double setTemp, double wattage);
	void initialRun();
	void changeDuty(int dutyCycle);
	double clamp(double n, double minn, double maxn);
	void closeHeater();
	double initialHeating;
private:
	int PWMPin;
};

#endif
