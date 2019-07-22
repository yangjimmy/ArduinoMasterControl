#include "Arduino.h"
#include "Heat.h"
#include "MAX31865.h"
#include <math.h>
//#include "DAC1284.h"
#include "AD5760.h"

Heat::Heat() {
	// empty constructor
}
/*
Heat::Heat(int MISO, int MOSI, int SCK, int LDAC_rtd, int LDAC_snipcard, int SS_rtd, int SS_snipcard, double _Kp, double _Ki, double _Kd, double _setPoint):

  rtd(MISO, MOSI, SCK, LDAC_rtd, SS_rtd),
  sc(SCK, SS_snipcard, MOSI),
  pid(dt, maxTemp, minTemp, Kp, Kd, Ki),
	setPoint(_setPoint),
  Kp(_Kp),
  Ki(_Ki),
  Kd(_Kd)
{
  Serial.println("new heat control");
}
*/

Heat::Heat(int _MISO, int _MOSI, int _SCK, int _LDAC_rtd, int _LDAC_dac, int _SS_rtd, int _SS_dac, double _Kp, double _Ki, double _Kd, double _setPoint)
{
	rtd = MAX31865(_MISO, _MOSI, _SCK, _LDAC_rtd, _SS_rtd);
	Serial.println("rtd");
	dac = AD5760(_MISO, _MOSI, _SCK, _LDAC_dac, _SS_dac);
	Serial.println("sc");
	pid = PID(dt, maxTemp, minTemp, _Kp, _Kd, _Ki);
	Serial.println("pid");
	setPoint = _setPoint;
	Serial.println("setPoint");
	Kp = _Kp;
	Ki = _Ki;
	Kd = _Kd;
}

void Heat::runHeat() {
	double presentTemperature = getTemperature();

	// calculate precentage of power required
	double presentPercentage = (presentTemperature - setPoint) / setPoint * 100.0;
	double adjustment = pid.calculate(0, presentPercentage);
	double newValue = presentPercentage + adjustment;
	setPower(newValue);

	// print the new temperature
	double newTemperature = newValue * setPoint + setPoint;
	//Serial.println(newTemperature);
}

void Heat::changeValues(double newKp, double newKi, double newKd, double newSetPoint) {
	Kp = newKp;
	Ki = newKi;
	Kd = newKd;
	setPoint = newSetPoint;
	pid = PID(dt, maxTemp, minTemp, Kp, Kd, Ki);
}

void Heat::setPower(double x) {
	// x=percentage of max power
	float voltageSend = (float)(4.6669 / (1 + pow(2.718281828459, (-0.0391 * (x - 39.8077)))));
	dac.setVoltage(voltageSend);
}

double Heat::getTemperature() {
	Serial.println("temperature read");
	return rtd.readTemp();
}
