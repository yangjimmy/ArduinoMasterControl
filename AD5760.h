// Driver written by Gokul
// With original Arduino program written by Zhengmin

#ifndef AD5760_h
#define AD5760_h
#include "Arduino.h"

class AD5760
{
	public:
		AD5760 (int MISO, int MOSI, int SCK, int LDAC, int SS);
		void initialize();
		void sendSequence(uint8_t first, uint8_t second, uint8_t third);
	private:
		static int MISO, MOSI, SCK, LDAC, SS;
		void sendByte(uint8_t value);
}

#endif