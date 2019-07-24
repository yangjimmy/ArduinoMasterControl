#include "Heater.h"
#include "Arduino.h"
#include "MAX31865.h"

Heater::Heater() {
	// empty constructor
}

Heater::Heater(int _PWMPin) {
	PWMPin = _PWMPin;
	pinMode(PWMPin, OUTPUT);
	analogWrite(PWMPin, 0);
}

double Heater::initialHeatingTime(double heatCapacity, double mass, double currTemp, double setTemp, double wattage) {
	//initialHeating = heatCapacity * mass * (setTemp - setTemp) / wattage;
  initialHeating = 8;
	return initialHeating; // in seconds
}

void Heater::initialRun() {
	analogWrite(PWMPin, 255);
}

void Heater::changeDuty(int dutyCycle) {
	analogWrite(PWMPin, dutyCycle);
}

double Heater::clamp(double n, double minn, double maxn) {
	return max(min(n, maxn), minn);
}

void Heater::closeHeater() {
	// set PWM duty cycle to 0
	analogWrite(PWMPin, 0);
}
