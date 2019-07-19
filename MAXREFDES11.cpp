#include "MAXREFDES11.h"
#include "Arduino.h"

MAXREFDES11::MAXREFDES11(){
  // empty constructor
}

MAXREFDES11::MAXREFDES11 (int param_MISO, int param_MOSI, int param_SCK, int param_LDAC, int param_SS) {
	_MISO = param_MISO;
	_MOSI = param_MOSI;
	_SCK = param_SCK;
	_SS = param_SS;
	_LDAC = param_LDAC;
	
	pinMode(_SS, OUTPUT);
  pinMode(_LDAC, OUTPUT);
	
	digitalWrite(_SS, HIGH);
  digitalWrite(_MOSI, LOW);
  digitalWrite(_SCK, LOW);
}

double MAXREFDES11::readVoltage(){
  Serial.println("voltage read");
  byte zeros;
  byte first;
  byte second;
  uint16_t value;
  double result;

  digitalWrite(_SS, LOW);
  zeros = receiveByte();
  first = receiveByte();
  second = receiveByte();
  digitalWrite(_SS, HIGH);
  delay(10);
  value = (first<<8) | second;

  result = ((value*10.0/65535.0) - 0.0001)/0.97675;
  return result;
}

byte MAXREFDES11::receiveByte(){
  byte result = 0x00;
  int i = 0;
  for (i; i<8; i++){
    digitalWrite(_SCK, HIGH);
    result = result << 1;
    if (digitalRead(_MISO)==HIGH){
      result = result | 0x1;
    }
    digitalWrite(_SCK, LOW);
  }
  return result;
}
