#include "AD5760.h"
#include "Pressure.h"
#include "Heat.h"

// Common pins
#define MISO 46 // MISO pin
#define MOSI 47 // MOSI pin
#define SCK 48 // clock pin
// Slave select pins
#define SS_ADC 49 // ADC slave select
#define SS_DAC 51 // DAC slave select
#define SS_RTD 53 // RTD Digital converter (MAX31865) slave select
#define SS_SC 45 // Snipcard slave select
// LDAC pins (test if they can be removed)
#define LDAC_ADC 50 // ADC LDAC
#define LDAC_DAC 52 // DAC LDAC
#define LDAC_RTD 54 // RTD Digital converter LDAC


// initialize variables
String inString = "";
Heat heatControl;
Pressure pressureControl;

int setPointPresIndex;
int pPresIndex;
int iPresIndex;
int dPresIndex;
double setPointPres;
double pPres;
double iPres;
double dPres;

int setPointTempIndex;
int pTempIndex;
int iTempIndex;
int dTempIndex;
double setPointTemp;
double pTemp;
double iTemp;
double dTemp;

bool hasSetPressure;
bool hasSetHeat;

// begin the program
void setup() {
  Serial.begin(9600);

  // set up pins
  pinMode(MISO, INPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(LDAC, OUTPUT);

  digitalWrite(MOSI, LOW);
  digitalWrite(SCK, LOW);
}

void loop() {
  readCommand();
  if (hasSetPressure=true){
    pressureControl.runPressure();
  }
  if (hasSetHeat=true){
    heatControl.runHeat();
  }
}


void readCommand() {
  if (Serial.available() > 0){
    int inChar = Serial.read();
    if (inChar == (int)'#'){
      beginRead();
    }
  }  
}

void beginRead(){
  while (Serial.available() > 0) {
    if (inChar != (int)'\n') {
      // As long as the incoming byte is not a newline, convert the incoming byte to a char and add it to the string
      Serial.println("incoming char");
      Serial.println((char)inChar);
      inString += (char)inChar;
      Serial.println("current string");
      Serial.println(inString);
    }
    // if you get a newline, decompose the string and perform the command
    else {
      String command = inString.substring(1,5);
      if (command == "SPRS"){
        int setPointPresIndex = inString.indexOf('S');
        int pPresIndex = inString.indexOf('P');
        int iPresIndex = inString.indexOf('I');
        int dPresIndex = inString.indexOf('D');
        double setPointPres = (inString.substring(setPointPresIndex,pIndex)).toDouble();
        double pPres = (inString.substring(pIndex,iIndex)).toDouble();
        double iPres = (inString.substring(iIndex,dIndex)).toDouble();
        double dPres = (inString.substring(dIndex,inString.length()).toDouble();
        if (hasSetPressure==false){
          pressureControl = Pressure(MISO, MOSI, SCK, LDAC, SS_DAC, SS_ADC, pPresIndex, iPresIndex, dPresIndex, setPointPres);
          hasSetPressure=true;
        }
        else {
          pressureControl.changeValues(pPresIndex, iPresIndex, dPresIndex, setPointPres);
        }
      } else if (command == "STMP"){
        int setPointTempIndex = inString.indexOf('S');
        int pTempIndex = inString.indexOf('P');
        int iTempIndex = inString.indexOf('I');
        int dTempIndex = inString.indexOf('D');
        double setPointTemp = (inString.substring(setPointTempIndex,pIndex)).toDouble();
        double pTemp = (inString.substring(pTempIndex,iTempIndex)).toDouble();
        double iTemp = (inString.substring(iTempIndex,dTempIndex)).toDouble();
        double dTemp = (inString.substring(dTempIndex,inString.length()).toDouble();
        if (hasSetHeat==false){
          heatControl = Heat(MISO, MOSI, SCK, LDAC, SS_RTD, SS_SC, pTempIndex, iTempIndex, dTempIndex, setPointTemp);
          hasSetHeat=true;
        }
        else {
          heatControl.changeValues(pTempIndex, iTempIndex, dTempIndex, setPointTemp);
        }
      } else if (command == "RPRS"){
        Serial.println(pressureControl.getPressure());
      } else if (command == "RTMP"){
        Serial.println(heatControl.getTemperature());
      } else if (command == "SVLT"){
        
      }
  }
}

// test
void setVoltage(float voltage) {
  uint16_t data = (uint16_t) (voltage * 65535 / 10);
  data = data & 0xffff;
  uint8_t first = 0b00010000 | (data >> 12);
  uint16_t second = (data << 4) & (0xffff);

  digitalWrite(SS_DAC, LOW);
  sendByte(first);
  sendByte((uint8_t)((second >> 8) & 0xff));
  sendByte((uint8_t)((second) & 0xf0));
  digitalWrite(SS_DAC, HIGH);
}

void sendByte(uint8_t value) {
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
