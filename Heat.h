// PID Heat Control
#ifndef HEAT_H
#define HEAT_H
#include "Arduino.h"
#include "Heat.h"
#include "MAX31865.h"
#include <math.h>
#include "PID.h"
//#include "SnipCard.h" // will borrow Gokul's code for the SnipCard

class Heat {
	public:
		Heat(int MISO, int MOSI, int SCK, int LDAC, int SS_rtd, double Kp, double Ki, double Kd, double setPoint);
		void runHeat();
		void changeValues(double newKp, double newKi, double newKd, double newSetPoint); 
		void setPower(double percent);
		double getTemperature();
	private:
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
