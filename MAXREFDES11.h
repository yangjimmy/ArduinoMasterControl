#ifndef MAXREFDES11_h
#define MAXREFDES11_h
#include "Arduino.h"

class MAXREFDES11
{
	public:
    MAXREFDES11 ();
		MAXREFDES11 (int _MISO, int _MOSI, int _SCK, int _LDAC, int _SS);
		double readVoltage();
	private:
		int MISO, MOSI, SCK, LDAC, SS;
		byte receiveByte();
};

#endif
