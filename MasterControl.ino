// Communication to/from Qt interface uses Gokul's protocol

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
// LDAC pins (test if they can be removed)
#define LDAC_ADC 50 // ADC LDAC
#define LDAC_DAC 52 // DAC LDAC
#define LDAC_RTD 54 // RTD Digital converter LDAC

#define PWM_PIN 10 // PWM pin


// initialize variables
Pressure *pressureControl = new Pressure();
Heat *heatControl = new Heat();

String inString = "";
String command = "";

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

bool hasSetPressure = false;
bool hasSetHeat = false;

bool finishedInitHeat = false;
double heatTime = 0;

unsigned long previousTime = 0;
unsigned long currentTime = 0;
unsigned long startTime = 0;

double currPres = 0;
double currTemp = 0;

String outString = "";

// begin the program
void setup() {
  Serial.begin(9600);
  // set up pins
  pinMode(MISO, INPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);

  digitalWrite(MOSI, LOW);
  digitalWrite(SCK, LOW);
}

void loop() {
  //int inChar;
  readCommand();
  /*
  while (Serial.available()) 
  {
    // get the new byte:
    inChar = (int)Serial.read();
    if (inChar == (int)'#'){
      while (inChar != (int)'\n'){
        inString+=(char)inChar;
      }
    }
  }
  if (inString!=""){
    Serial.println(inString);
  }
  inString = "";
  */
  //Serial.println("P12T10");
  
  
  
  inString = ""; // reset inString to prepare for new command
  
  //Serial.println(heatControl->getTemperature());
  if (hasSetPressure == true) {
	  currentTime = micros();
	  (*pressureControl).runPressure();
    //pressureControl->readPressure(); TO DO: Spit out pressure value
    currPres = pressureControl->getPressure();
	  previousTime = currentTime;
  }
  if (hasSetHeat == true) {
	  currentTime = micros();
	  if (finishedInitHeat == false) {
      // initial rampup
		  heatControl->runInitial(&startTime, previousTime, currentTime, &finishedInitHeat);
      currTemp = heatControl->getTemperature();
	  }
	  else if (finishedInitHeat && ((double)(abs(currentTime-startTime)/1000000.0) < heatTime)){
      // attempt to maintain temperature
		  heatControl->runHeat(previousTime, currentTime);
      currTemp = heatControl->getTemperature();
	  } else if (finishedInitHeat && (double)(abs(currentTime-startTime)/1000000.0) > heatTime) {
      heatControl->stopHeat(); // PWM to zero
      hasSetHeat = false; // reset the system
      finishedInitHeat = false;      
	  } else {
      Serial.println("Parse error");
	  }
	  previousTime = currentTime;
  }
  /*
  outString += "P";
  outString += (int)currPres*100;
  outString += "T";
  outString += (int)currTemp*100;
  Serial.println(outString);
  outString="";
  */
  
}

void doNothing() {
  // do nothing
}

void readCommand() {
  //Serial.println("In command");
  if (Serial.available() > 0) {
    //Serial.println("In serial available");
    
    int inChar = Serial.read();
    if (inChar == (int)'#') {
      //Serial.println("cmd registered");
      beginRead();
      return;
    } else {
      return;
    } 
  }
  //return;
  //return -1;
}
/*
void beginRead() {
  
}
*/

void beginRead() {
  delay(5);
  while (Serial.available() > 0) {
    //Serial.println("Available");
    int inChar = Serial.read();
    if (inChar != (int)'\n') {
      // As long as the incoming byte is not a newline, convert the incoming byte to a char and add it to the string
      inString += (char)inChar;
    }
    // if you get a newline, decompose the string and perform the command
    else {
      break;
    }
    delay(5);
  }
  //Serial.println(inString);
  //Serial.println("Done Read");
  //String command = inString.substring(0, 4);
  command = inString.substring(0, 4);
  //Serial.println(command);
  if (command == "SPRS") {
    int setPointPresIndex = inString.lastIndexOf('S');
    //Serial.println(setPointPresIndex);
    int pPresIndex = inString.lastIndexOf('P');
    //Serial.println(pPresIndex);
    int iPresIndex = inString.lastIndexOf('I');
    //Serial.println(iPresIndex);
    int dPresIndex = inString.lastIndexOf('D');
    //Serial.println(dPresIndex);
    double setPointPres = (double)((inString.substring(setPointPresIndex + 1, pPresIndex)).toFloat());
    //Serial.println("SetPoint Value");
    //Serial.println(setPointPres);
    double pPres = (double)((inString.substring(pPresIndex + 1, iPresIndex)).toFloat());
    //Serial.println("Pressure P Value");
    //Serial.println(pPres);
    double iPres = (double)((inString.substring(iPresIndex + 1, dPresIndex)).toFloat());
    //Serial.println("Pressure I Value");
    //Serial.println(iPres);
    double dPres = (double)((inString.substring(dPresIndex + 1, inString.length())).toFloat());
    //Serial.println("Pressure D Value");
    //Serial.println(dPres);
    if (hasSetPressure == false) {
      //Serial.println("has set pressure is false");
      free(pressureControl);
      //Serial.println("new pressure control");
      pressureControl = new Pressure(MISO, MOSI, SCK, LDAC_DAC, LDAC_ADC, SS_DAC, SS_ADC, pPres, iPres, dPres, setPointPres);
      hasSetPressure = true;
	    previousTime = micros();
    }
    else {
      //Serial.println("has pressure control");
      (*pressureControl).changeValues(pPresIndex, iPresIndex, dPresIndex, setPointPres);
    }
    return;
  } else if (command == "STMP") {
    int setPointTempIndex = inString.lastIndexOf('S');
    //Serial.println(setPointTempIndex);
    int pTempIndex = inString.lastIndexOf('P');
    //Serial.println(pTempIndex);
    int iTempIndex = inString.lastIndexOf('I');
    //Serial.println(iTempIndex);
    int dTempIndex = inString.lastIndexOf('D');
    //Serial.println(dTempIndex);
    int tTempIndex = inString.lastIndexOf('T');
    //Serial.println(tTempIndex);
    
    double setPointTemp = (double)((inString.substring(setPointTempIndex + 1, pTempIndex)).toDouble());
    //Serial.println("SetPoint Value");
    //Serial.println(setPointTemp);
    double pTemp = (double)((inString.substring(pTempIndex + 1, iTempIndex)).toFloat());
    //Serial.println("Temperature P Value");
    //Serial.println(pTemp);
    double iTemp = (double)((inString.substring(iTempIndex + 1, dTempIndex)).toFloat());
    //Serial.println("Temperature I Value");
    //Serial.println(iTemp);
    //double dTemp = (double)((inString.substring(dTempIndex + 1, inString.length())).toFloat());
    double dTemp = (double)((inString.substring(dTempIndex + 1, tTempIndex)).toFloat());
    //Serial.println("Temperature D Value");
    //Serial.println(dTemp);
    heatTime = (double)((inString.substring(tTempIndex + 1, inString.length())).toFloat());
    //Serial.println("Heat Time Value");
    //Serial.println("-----------heat time-----------");
    //Serial.println(heatTime);
    if (hasSetHeat == false) {
      //Serial.println("has set heat is false");
      free(heatControl);
      //Serial.println("new heat control");
	    heatControl = new Heat(MISO, MOSI, SCK, LDAC_RTD, SS_RTD, PWM_PIN, pTemp, iTemp, dTemp, setPointTemp);
      //Serial.println("To next");
      hasSetHeat = true;
	    startTime = micros();
      //Serial.print("---------start time----------");
      //Serial.println(startTime);
	    previousTime = micros();
    }
    else {
      //Serial.println("has set heat is true");
      (*heatControl).changeValues(pTemp, iTemp, dTemp, setPointTemp);
    }
    return;
  } else if (command == "RPRS") {
    Serial.println((*pressureControl).getPressure());
    return;
  } else if (command == "RTMP") {
    //Serial.println("RTMP");
    Serial.println(heatControl->getTemperature());
    return;
  } else if (command == "SVLT") {
    return;
  }
}
