#ifndef Pressure_h
#define Pressure_h
#include "Arduino.h"
#include "AD5760.h"
#include "MAXREFDES11.h"
#include "PID_v1.h"

class Pressure
{
	public:
    Pressure();
		Pressure (int MISO, int MOSI, int SCK, int LDAC_dac, int LDAC_adc, int SS_dac, int SS_adc, double Kp, double Ki, double Kd, double setPoint);
		  //:dac(MISO, MOSI, SCK, LDAC_dac, SS_dac), adc(MISO, MOSI, SCK, LDAC_adc, SS_adc), pid(dt, maxPressure, minPressure, Kp, Kd, Ki), Kp(_Kp), Ki(_Ki), Kd(_Kd), setPoint(_setPoint);
		void runPressure();
		void changeValues(double newKp, double newKi, double newKd, double newSetPoint);
		void setPressure(float pressure);
    void getDACValues();
    void getSelfValues();
		double getPressure();
	private:
		AD5760 dac;
		MAXREFDES11 adc;
		
		double Kp = 0;
		double Ki = 0;
		double Kd = 0;
		double *setPoint = 0;
    double *output = 0;
    double *input = 0;
    PID pid = PID(input, output, setPoint, Kp, Ki, Kd, DIRECT);
    
		const double maxPressure = 36; // max pressure in bars (atms)
		const double minPressure = -2; // min pressure in bars (atms)
		const double dt = 1; // 1 second refresh to give system time to adjust
};

#endif
