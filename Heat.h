// PID Heat Control
#ifndef HEAT_H_
#define HEAT_H_
#include "Arduino.h"
#include "Heat.h"
#include "MAX31865.h"
#include <math.h>
#include "PID_v1.h"
#include "Heater.h"
#include "PID_AutoTune_v0.h"

class Heat {
	public:
		Heat();
		//Heat(int _MISO, int _MOSI, int _SCK, int _LDAC_rtd, int _LDAC_snipcard, int _SS_rtd, int _SS_snipcard, double _Kp, double _Ki, double _Kd, double _setPoint);
		Heat(int _MISO, int _MOSI, int _SCK, int _LDAC_rtd, int _SS_rtd, int _PWMPin, double _Kp, double _Ki, double _Kd, double _setPoint);
		void runInitial(unsigned long *startTime, unsigned long previousTime, unsigned long currentTime, bool *finished);
		void runHeat(unsigned long previous, unsigned long current);
		void changeValues(double newKp, double newKi, double newKd, double newSetPoint); 
		void setPower(double percent);
		double getTemperature();
    void stopHeat();
	private:
		Heater heater;
		MAX31865 rtd;
		double Kp = 0;
		double Ki = 0;
		double Kd = 0;
		double *setPoint = 0;
    double *input = 0;
    double *output = 0;
		PID pid = PID(input, output, setPoint, Kp, Ki, Kd, DIRECT);
    PID_ATune aTune = PID_ATune(input, output);
    bool tuning = false;
		const double maxTemp = 150; // max temperature in deg Celsius
		const double minTemp = 10; // min pressure in deg Celsius
		double dt = 0;

		const double heatCapacity = 0.91;
		const double mass = 1.715;
		const double wattage = 2.60;

    void AutoTuneHelper(bool start);
    byte ATuneModeRemember=2;
};

#endif
