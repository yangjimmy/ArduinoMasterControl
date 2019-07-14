#include "MAXREFDES11.h"
#include "Arduino.h"

MAXREFDES11::MAXREFDES11 (int MISO, int MOSI, int SCK, int LDAC, int SS) {
	this.MISO = MISO;
	this.MOSI = MOSI;
	this.SCK = SCK;
	this.SS = SS;
	this.LDAC = LDAC;
	
	pinMode(this.SS, OUTPUT);
	
	digitalWrite(SS, HIGH);
  digitalWrite(MOSI, LOW);
  digitalWrite(SCK, LOW);
}

double MAXREFDES11::readVoltage(){
  byte zeros;
  byte first;
  byte second;
  uint16_t value;
  double result;

  digitalWrite(SS, LOW);
  zeros = receiveByte();
  first = receiveByte();
  second = receiveByte();
  digitalWrite(SS, HIGH);
  delay(10);
  value = (first<<8) | second;

  result = ((value*10.0/65535.0) - 0.0001)/0.97675;
  return result;
}

byte MAXREFDES11::receiveByte(){
  byte result = 0x00;
  int i = 0;
  for (i; i<8; i++){
    digitalWrite(SCK, HIGH);
    result = result << 1;
    if (digitalRead(MISO)==HIGH){
      result = result | 0x1;
    }
    digitalWrite(SCK, LOW);
  }
  return result;
}
