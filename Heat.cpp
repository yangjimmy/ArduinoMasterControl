#include "Arduino.h"
#include "Heat.h"
#include "MAX31865.h"
#include <math.h>
#include "AD5760.h"
#include "Heater.h"
#include <PID_v1.h>
#include "PID_AutoTune_v0.h"

Heat::Heat() {
	// empty constructor
}

Heat::Heat(int _MISO, int _MOSI, int _SCK, int _LDAC_rtd, int _SS_rtd, int _PWMPin, double _Kp, double _Ki, double _Kd, double _setPoint)
{
	rtd = MAX31865(_MISO, _MOSI, _SCK, _LDAC_rtd, _SS_rtd);
	heater = Heater(_PWMPin);
	input = rtd.readTemp();
	heater.initialHeatingTime(heatCapacity, mass, input, _setPoint, wattage);

	setPoint = _setPoint;
	
	Kp = _Kp;
	Ki = _Ki;
	Kd = _Kd;

  pid.SetTunings(Kp, Ki, Kd);
  // k values ok
  pid.SetMode(AUTOMATIC);
}

void Heat::runInitial(unsigned long *startTime, unsigned long previousTime, unsigned long currentTime, bool *finished) {
	if ((currentTime-*startTime)/1000000.0 < heater.initialHeating) {
    //Serial.println(heater.initialHeating);
		heater.initialRun();
	}
	else {
    //Serial.println(heater.initialHeating);
    //Serial.println("Finished init");
		*finished = true;
		heater.changeDuty(0);
    *startTime = currentTime;
	}
}


void Heat::runHeat(unsigned long previous, unsigned long current) {
	//double presentTemperature = getTemperature();
	input = getTemperature();
    
  // autotune //
  
  if(tuning)
  {
    //Serial.println("tuning");
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
//      Serial.println("k vals after tuning");
//      Serial.println(Kp);
//      Serial.println(Ki);
//      Serial.println(Kd);
//      Serial.println("---");
      pid.SetTunings(Kp,Ki,Kd);
      AutoTuneHelper(false);
    }
  }
  else {
    pid.Compute();
  }
  
  //pid.Compute();
  
	// calculate precentage of power required
	
	double newDuty = output;
 
	setPower(newDuty);
//  Serial.println("output");
//  Serial.println(output);
//  Serial.println("end output");
}

void Heat::changeValues(double newKp, double newKi, double newKd, double newSetPoint) {
	Kp = newKp;
	Ki = newKi;
	Kd = newKd;
	setPoint = newSetPoint;
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
		heater.changeDuty((int)(x));
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
