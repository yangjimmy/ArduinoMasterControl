#ifndef MAX31865_h
#define MAX31865_h
#include "Arduino.h"
#include <math.h>

class MAX31865
{
	public:
    MAX31865 ();
		MAX31865 (int param_MISO, int param_MOSI, int param_SCK, int param_LDAC, int param_SS);
		double readTemp();
	private:
		int _MISO, _MOSI, _SCK, _LDAC, _SS;
		void writeRegister(uint8_t regNumber, uint8_t data);
		void readRegisters(uint8_t regNumStart, int numRegisters, byte *data_out);
		void sendByte(uint8_t value);
		byte receiveByte();
		double calcTemp(uint16_t rtd_ADC_Code);
};

#endif
