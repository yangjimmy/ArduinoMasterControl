#include "Pressure.h"
#include "Arduino.h"
#include "AD5760.h"
#include "MAXREFDES11.h"
#include "PID_v1.h"

Pressure::Pressure()
{
  // empty constructor
}

/*
Pressure::Pressure(int MISO, int MOSI, int SCK, int LDAC_dac, int LDAC_adc, int SS_dac, int SS_adc, double _Kp, double _Ki, double _Kd, double _setPoint):
	dac(MISO, MOSI, SCK, LDAC_dac, SS_dac),
	adc(MISO, MOSI, SCK, LDAC_adc, SS_adc),
	pid(dt, maxPressure, minPressure, Kp, Kd, Ki),
  Kp(_Kp),
  Ki(_Ki),
  Kd(_Kd),
  setPoint(_setPoint)
{
}
*/

Pressure::Pressure(int MISO, int MOSI, int SCK, int LDAC_dac, int LDAC_adc, int SS_dac, int SS_adc, double _Kp, double _Ki, double _Kd, double _setPoint)
{
  dac = AD5760(MISO, MOSI, SCK, LDAC_dac, SS_dac);
  adc = MAXREFDES11(MISO, MOSI, SCK, LDAC_adc, SS_adc);
  //pid = PID(dt, maxPressure, minPressure, Kp, Kd, Ki);
  Kp = _Kp;
  Ki = _Ki;
  Kd = _Kd;
  *setPoint = _setPoint;
  *input = getPressure();
  pid.SetTunings(Kp, Ki, Kd);
  pid.SetOutputLimits(0, 35);
  pid.SetMode(AUTOMATIC);
}

void Pressure::getDACValues(){
  dac.getValues();
}

void Pressure::getSelfValues(){
  Serial.println(Kp);
  Serial.println(Ki);
  Serial.println(Kd);
  Serial.println(*setPoint);
}

void Pressure::changeValues(double newKp, double newKi, double newKd, double newSetPoint)
{
  if (newSetPoint > 35) {
    //Serial.println("Error, resetting to 35");
    *setPoint = 35;
  } else if (newSetPoint < 0) {
    //Serial.println("Error, resetting to 0");
    *setPoint = 0;
  } else {
    *setPoint = newSetPoint;
  }
  Kp = newKp;
  Ki = newKi;
  Kd = newKd;
  pid.SetTunings(Kp, Ki, Kd);
}

void Pressure::runPressure()
{
	// no error
	// run the value through the PID and set a new pressure based on PID results
	//double presentValue = getPressure();
	//double adjustment = pid.calculate(setPoint, presentValue);
	//double newValue = presentValue + adjustment;
  *input = getPressure();
  pid.Compute();
	setPressure(*output);
}

void Pressure::setPressure(float pressure) {
	// error check
	if (pressure > 37) {
		//Serial.println("Error, resetting to 35");
		pressure = 35;
	}
	if (pressure < -2) {
		//Serial.println("Error, resetting to 0");
		pressure = 0;
	}
	uint16_t data = (uint16_t)((pressure + 0.3)/35*65535);
	data = data & 0xffff;
	uint8_t first = 0b00010000 | (data >> 12);
	uint16_t second = (data << 4) & (0xffff);
  
	dac.sendSequence(first, (uint8_t)((second >> 8) & 0xff), (uint8_t)((second) & 0xf0));
}

double Pressure::getPressure() {
  Serial.println("pressure read");
	double voltage = adc.readVoltage();
	return voltage/10*35;
}
