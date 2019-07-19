// PID Heat Control
#ifndef HEAT_H_
#define HEAT_H_
#include "Arduino.h"
#include "Heat.h"
#include "MAX31865.h"
#include <math.h>
#include "PID.h"
#include "DAC1284.h"

class Heat {
	public:
    Heat();
		Heat(int _MISO, int _MOSI, int _SCK, int _LDAC_rtd, int _LDAC_snipcard, int _SS_rtd, int _SS_snipcard, double _Kp, double _Ki, double _Kd, double _setPoint);
		void runHeat();
		void changeValues(double newKp, double newKi, double newKd, double newSetPoint); 
		void setPower(double percent);
		double getTemperature();
	private:
    DAC1284 sc;
		MAX31865 rtd;
		double Kp;
		double Ki;
		double Kd;
		double setPoint;
		PID pid;
		const double maxTemp = 150; // max temperature in deg Celsius
		const double minTemp = 10; // min pressure in deg Celsius
		const double dt = 0.01; // 1 second refresh to give system time to adjust
};

#endif
