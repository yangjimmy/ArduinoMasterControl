// Driver written by Gokul
// With original Arduino program written by Zhengmin

#include "AD5760.h"
#include "Arduino.h"

AD5760::AD5760 (int _MISO, int _MOSI, int _SCK, int _LDAC, int _SS) {
	MISO = _MISO;
  MOSI = _MOSI;
  SCK = _SCK;
  SS = _SS;
  LDAC = _LDAC;
	
	pinMode(SS, OUTPUT);
	
	digitalWrite(SS, HIGH);
    digitalWrite(MOSI, LOW);
    digitalWrite(SCK, LOW);
	initialize();
}

void AD5760::sendByte(uint8_t value) {
  int i = 0;
  for (i; i < 8; i++) {
    digitalWrite(SCK, HIGH);
    if ((value & 0x80) == 0x80) {
      digitalWrite(MOSI, HIGH);
    } else {
      digitalWrite(MOSI, LOW);
    }
    value = value << 1;
    digitalWrite(SCK, LOW);
  }
}

void AD5760::sendSequence(uint8_t first, uint8_t second, uint8_t third) {
	digitalWrite(SS, LOW);
    sendByte(first);
    sendByte(second);
    sendByte(third);
    digitalWrite(SS, HIGH);
}

void AD5760::initialize() {
  uint8_t software_high = 0b01000000;
  uint8_t software_mid = 0b00000000;
  uint8_t software_low = 0b00000000;
  sendSequence(software_high, software_mid, software_low);

  uint8_t control_high = 0b00100000;
  uint8_t control_mid = 0b00000000;
  uint8_t control_low = 0b00010010;
  sendSequence(control_high, control_mid, control_low);
}
