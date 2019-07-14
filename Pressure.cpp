#include "Pressure.h"
#include "Arduino.h"
#include "AD5760.h"
#include "MAXREFDES11.h"
#include "PID.h"

Pressure::Pressure(int MISO, int MOSI, int SCK, int LDAC, int SS_dac, int SS_adc, double Kp, double Ki, double Kd, double setPoint)
{
	dac = AD5760(MISO, MOSI, SCK, LDAC, SS_dac);
	adc = MAXREFDES11(MISO, MOSI, SCK, SS_adc);
	this.Kp = Kp;
	this.Ki = Ki;
	this.Kd = Kd;
	this.setPoint = setPoint;
	pid = PID(dt, maxPressure, minPressure, Kp, Kd, Ki);
}

void Pressure::changeValues(double newKp, double newKi, double newKd, double newSetPoint)
{
  Kp = newKp;
  Ki = newKi;
  Kd = newKd;
	setPoint = newSetPoint;
  pid = PID(dt, maxPressure, minPressure, Kp, Kd, Ki);
}

void Pressure::runPressure()
{
	if (pressure > 35) {
		Serial.println("Error, resetting to 35");
		pressure = 35;
	} else if (pressure < 0) {
		Serial.println("Error, resetting to 0");
		pressure = 0;
	} else {
		// no error
		// run the value through the PID and set a new pressure based on PID results
		double presentValue = getPressure();
		double adjustment = pid.calculate(setPoint, presentValue);
		double newValue = presentValue + adjustment;
		setPressure(newValue);
	}
}

void Pressure::setPressure(float pressure) {
	// error check
	if (pressure > 37) {
		Serial.println("Error, resetting to 35");
		pressure = 35;
	}
	if (pressure < -2) {
		Serial.println("Error, resetting to 0");
		pressure = 0;
	}
	uint16_t data = (pressure + 0.3)/35*65535;
	data = data & 0xffff;
	uint8_t first = 0b00010000 | (data >> 12);
	uint16_t second = (data << 4) & (0xffff);
  
	dac.sendSequence(first, (uint8_t)((second >> 8) & 0xff), (uint8_t)((second) & 0xf0));
}

double Pressure::getPressure() {
	double voltage = adc.readVoltage();
	return voltage/10*35;
}
