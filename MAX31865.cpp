#include "Arduino.h"
#include <math.h>
#include "MAX31865.h"

MAX31865::MAX31865 (){
  // empty
}

MAX31865::MAX31865 (int _MISO, int _MOSI, int _SCK, int _LDAC, int _SS) {
	MISO = _MISO;
  MOSI = _MOSI;
  SCK = _SCK;
  SS = _SS;
  LDAC = _LDAC;
	
	pinMode(SS, OUTPUT);
  pinMode(LDAC, OUTPUT);
	
	digitalWrite(SS, HIGH);
  digitalWrite(MOSI, LOW);
  digitalWrite(SCK, LOW);
}

double MAX31865::readTemp(){
  writeRegister(0, 0xA2);
  delay(100);
  byte out [8];

  readRegisters(0,8,out);
  
  uint8_t conf_reg = (uint8_t)out[0];
  
  uint8_t rtd_msb = out[1];
  uint8_t rtd_lsb = out[2];
  uint16_t rtd_ADC_Code = ((rtd_msb<<8)|rtd_lsb)>>1;

  double temp_C = calcTemp(rtd_ADC_Code);

  uint8_t hft_msb = out[3];
  uint8_t hft_lsb = out[4];
  uint16_t hft = ((hft_msb<<8)|hft_lsb)>>1;

  uint8_t lft_msb = out[3];
  uint8_t lft_lsb = out[4];
  uint16_t lft = ((lft_msb<<8)|lft_lsb)>>1;

  uint8_t curr_status = out[7];

  if ((curr_status & 0x80) == 0x80){
    //Serial.println("High threshold limit");
    return -1;
  } else if ((curr_status & 0x40) == 0x40){
    //Serial.println("Low threshold limit");
    return -1;
  } else if ((curr_status & 0x04) == 0x04){
    //Serial.println("Overvoltage/Undervoltage");
    return -1;
  } else {
    //Serial.println(temp_C);
    return temp_C;
  }
}

void MAX31865::writeRegister(uint8_t regNumber, uint8_t data){
  digitalWrite(SS, LOW);
  uint8_t first = (0x80)|(regNumber);
  uint8_t second = data & 0xff;
  sendByte(first);
  sendByte(second);
  digitalWrite(SS, HIGH);
}

void MAX31865::readRegisters(uint8_t regNumStart, int numRegisters, byte *data_out){
  digitalWrite(SS, LOW);
  sendByte(regNumStart);
  int i = 0;
  for (i; i<numRegisters; i++){
    data_out[i] = receiveByte();
  }
  //Serial.println("Done readRegisters");
  digitalWrite(SS, HIGH);
}

void MAX31865::sendByte(uint8_t value){
  int i = 0;
  for (i; i<8; i++){
    digitalWrite(SCK, HIGH);
    if ((value & 0x80)==0x80){
      digitalWrite(MOSI, HIGH);
    } else {
      digitalWrite(MOSI, LOW);
    }
    value = value << 1;
    digitalWrite(SCK, LOW);
  }
}

byte MAX31865::receiveByte(){
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

double MAX31865::calcTemp(uint16_t rtd_ADC_Code){
  const float R_REF = 428.8;
  const float Res0 = 100;
  const float a = 0.00390830;
  const float b = -0.000000577500;
  const float c = 0;

  float res_RTD = rtd_ADC_Code*R_REF/32768.0;

  double temp_C = -(a*Res0) + sqrt(a*a*Res0*Res0 - 4*(b*Res0)*(Res0 - res_RTD));
  temp_C = temp_C / (2*(b*Res0));

  if (temp_C < 0){
    //Serial.println("Negative temperature");
    temp_C = (rtd_ADC_Code/32) - 256;
  }

  return temp_C;
}
