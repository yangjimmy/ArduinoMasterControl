#ifndef MAXREFDES11_h
#define MAXREFDES11_h
#include "Arduino.h"

class MAXREFDES11
{
	public:
		MAXREFDES11 (int MISO, int MOSI, int SCK, int LDAC, int SS);
		double readVoltage();
	private:
		static int MISO, MOSI, SCK, LDAC, SS;
		void receiveByte();
}

#endif