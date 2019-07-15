#ifndef MAX31865_h
#define MAX31865_h
#include "Arduino.h"
#include <math.h>

class MAX31865
{
	public:
    MAX31865 ();
		MAX31865 (int _MISO, int _MOSI, int _SCK, int _LDAC, int _SS);
		double readTemp();
	private:
		int MISO, MOSI, SCK, LDAC, SS;
		void writeRegister(uint8_t regNumber, uint8_t data);
		void readRegisters(uint8_t regNumStart, int numRegisters, byte *data_out);
		void sendByte(uint8_t value);
		byte receiveByte();
		double calcTemp(uint16_t rtd_ADC_Code);
};

#endif
