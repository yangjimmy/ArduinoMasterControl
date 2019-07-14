#ifndef Pressure_h
#define Pressure_h
#include "Arduino.h"
#include "AD5760.h"
#include "MAXREFDES11.h"
#include "PID.h"

class Pressure
{
	public:
		Pressure (int MISO, int MOSI, int SCK, int LDAC, int SS_dac, int SS_adc, double Kp, double Ki, double Kd, double setPoint);
		void runPressure();
		void changeValues(double newKp, double newKi, double newKd, double newSetPoint);
		void setPressure(float pressure);
		double getPressure();
	private:
		AD5760 dac;
		MAXREFDES11 adc;
		PID pid;
		double Kp;
		double Ki;
		double Kd;
		double setPoint;
		const double maxPressure = 36; // max pressure in bars (atms)
		const double minPressure = -2; // min pressure in bars (atms)
		const double dt = 1 // 1 second refresh to give system time to adjust
}

#endif
