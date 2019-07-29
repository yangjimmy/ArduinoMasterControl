#include "Arduino.h"
#include "Heat.h"
#include "MAX31865.h"
#include <math.h>
//#include "DAC1284.h"
#include "AD5760.h"
#include "Heater.h"
#include "PID_v1.h"
#include "PID_AutoTune_v0.h"

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

/*
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
*/

Heat::Heat(int _MISO, int _MOSI, int _SCK, int _LDAC_rtd, int _SS_rtd, int _PWMPin, double _Kp, double _Ki, double _Kd, double _setPoint)
{
	rtd = MAX31865(_MISO, _MOSI, _SCK, _LDAC_rtd, _SS_rtd);
	heater = Heater(_PWMPin);
	*input = rtd.readTemp();
  double currTemp = rtd.readTemp();
	heater.initialHeatingTime(heatCapacity, mass, currTemp, _setPoint, wattage);
	
	
	
	*setPoint = _setPoint;
	
	Kp = _Kp;
	Ki = _Ki;
	Kd = _Kd;

  pid.SetTunings(Kp, Ki, Kd);
  pid.SetOutputLimits(minTemp, maxTemp);
  pid.SetMode(AUTOMATIC);

  //aTune = PID_ATune(input, output);
}

void Heat::runInitial(unsigned long *startTime, unsigned long previousTime, unsigned long currentTime, bool *finished) {
	if ((currentTime-*startTime)/1000000.0 < heater.initialHeating) {
		heater.initialRun();
	}
	else {
    Serial.println("Finished init");
		*finished = true;
		heater.changeDuty(0);
    *startTime = currentTime;
    Serial.println(*startTime);
    Serial.println(currentTime);
	}
}


void Heat::runHeat(unsigned long previous, unsigned long current) {
	//double presentTemperature = getTemperature();
	*input = getTemperature();
	//dt = (double)((current - previous)/1000000.0); // convert to seconds and double type
  
	//pid = PID(dt, maxTemp, minTemp, Kp, Kd, Ki);
  
  // autotune //
  if(tuning)
  {
    byte val = (aTune.Runtime());
    if (val!=0)
    {
      tuning = false;
    }
    if(!tuning)
    { //we're done, set the tuning parameters
      Kp = aTune.GetKp();
      Ki = aTune.GetKi();
      Kd = aTune.GetKd();
      pid.SetTunings(Kp,Ki,Kd);
      AutoTuneHelper(false);
    }
  }
  else {
    pid.Compute();
  }

  
	// calculate precentage of power required
	//double presentPercentage = (presentTemperature - setPoint) / setPoint * 100.0;
	//double newPercentage = pid.calculate(0, presentPercentage);
	//double newValue = presentPercentage + adjustment;
	//double newDuty = pid.calculate(setPoint, presentTemperature);
	double newDuty = *output;
	setPower(newDuty);

	// print the new temperature
	//double newTemperature = newValue * setPoint + setPoint;
	//Serial.println(newTemperature);
}

void Heat::changeValues(double newKp, double newKi, double newKd, double newSetPoint) {
	Kp = newKp;
	Ki = newKi;
	Kd = newKd;
	*setPoint = newSetPoint;
	//pid = PID(dt, maxTemp, minTemp, Kp, Kd, Ki);
  pid.SetTunings(Kp, Ki, Kd);
}

void Heat::setPower(double x) {
	// x=duty cycle

	//float voltageSend = (float)(4.6669 / (1 + pow(2.718281828459, (-0.0391 * (x - 39.8077)))));
	//dac.setVoltage(voltageSend);
	if (x < 0) {
		heater.changeDuty(0);
	}
	else {
		heater.changeDuty((int)(x / 100 * 255));
	}
}

double Heat::getTemperature() {
	//Serial.println("temperature read");
	return rtd.readTemp();
}

void Heat::stopHeat(){
  //Serial.println("Stopping");
  heater.closeHeater();
}

void Heat::AutoTuneHelper(bool start)
{
  if(start)
    ATuneModeRemember = pid.GetMode();
  else
    pid.SetMode(ATuneModeRemember);
}
