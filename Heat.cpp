#include "Arduino.h"
#include "Heat.h"
#include "MAX31865.h"
#include <math.h>
//#include "SnipCard.h" // will borrow Gokul's code for the SnipCard
#define en 2.718281828459

class Heat
{
	Heat::Heat(int MISO, int MOSI, int SCK, int LDAC, int SS_rtd, int SS_snipcard, double Kp, double Ki, double Kd, double _setPoint)
	{
		rtd = MAX31865(MISO, MOSI, SCK, LDAC, SS_rtd);
    snipcard = SnipCard(MISO, MOSI, SCK, LDAC, SS_snipcard);
		pid = PID(dt, maxTemp, minTemp, Kp, Kd, Ki);
		setPoint = _setPoint;
	}
	
	void Heat::runHeat() {
		double presentTemperature = getTemperature();
    // calculate precentage of power required
    double presentPercentage = (presentTemperature-setPoint)/setPoint*100.0;
		double adjustment = pid.calculate(0, presentPercentage);
		double newValue = presentValue + adjustment;
		setPower(newValue);
		
		// print the new temperature
		double newTemperature = newValue*setPoint+setPoint;
		Serial.println(newTemperature);
	}
	
	void Heat::changeValues(double newKp, double newKi, double newKd, double newSetPoint) {
		Kp = newKp;
    Ki = newKi;
    Kd = newKd;
    setPoint = newSetPoint;
    pid = PID(dt, maxPressure, minPressure, Kp, Kd, Ki);
	}
	
	void Heat::setPower(double x) {
		// x=percentage of max power
    /*
		static const double a0 = -4.427e+10;
		static const double a1 = 6.769e+10;
		static const double b1 = 4.08e+10;
		static const double a2 = -2.616e+10;
		static const double b2 =  -4.952e+10;
		static const double a3 = -1.759e+09 ;
		static const double b3 = 3.114e+10;
		static const double a4 = 7.539e+09;
		static const double b4 = -1.105e+10;
		static const double a5 = -3.883e+09;
		static const double b5 = 1.779e+09;
		static const double a6 = 9.519e+08;
		static const double b6 = 1.077e+08;
		static const double a6 = -1.069e+08;
		static const double b7 = -8.211e+07;
		static const double a8 = 3.275e+06;
		static const double b8 = 8.35e+06;
		static const double w = 0.01053;
		
		double voltageSend = a0 + a1*cos(x*w) + b1*sin(x*w) + 
               a2*cos(2*x*w) + b2*sin(2*x*w) + a3*cos(3*x*w) + b3*sin(3*x*w) + 
               a4*cos(4*x*w) + b4*sin(4*x*w) + a5*cos(5*x*w) + b5*sin(5*x*w) + 
               a6*cos(6*x*w) + b6*sin(6*x*w) + a7*cos(7*x*w) + b7*sin(7*x*w) + 
               a8*cos(8*x*w) + b8*sin(8*x*w);
    */

    double voltageSend = 4.6669/(1+pow(eu,(-0.0391*(x-39.8077))));
    // TODO: send voltage to snipCard
	}
	
	double getTemperature() {
		return rtd.calcTemp();
	}
}
