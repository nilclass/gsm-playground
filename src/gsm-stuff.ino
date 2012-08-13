/* -*- mode:c++ -*- */

#include <stdarg.h>

#include <avr/pgmspace.h>

#include "gsm-stuff.h"

#define INIT_DELAY 3000
#define SAMPLE_DELAY 60000
#define SAMPLE_COUNT 60

bool fullyInitialized = false;
GSM gsm;
float *tempValues[SAMPLE_COUNT];
unsigned int tempCounter = 0;

bool halted = false;

void mallocFailed() {
  Serial.println("No memory, dude!");
  halted = true;
}

void printRegistrationState(int reg) {
#ifdef DEBUG_GSM
  switch (reg){
  case REG_NOT_REGISTERED:
    Serial.println("not registered");
    break;
  case REG_REGISTERED:
    Serial.println("GSM module is registered");
    break;
  case REG_NO_RESPONSE:
    Serial.println("GSM doesn't respond");
    break;
  case REG_COMM_LINE_BUSY:
    Serial.println("comm line is not free");
    break;
  }
#endif
}

void prepareTempValues() {
  int sensorCount = Sensors::count();

  for(int i=0;i<SAMPLE_COUNT;i++) {
    if(! (tempValues[i] = (float*) malloc(sensorCount * sizeof(float)))) {
      mallocFailed();
      return;
    }
  }
}

void setup() {

  Serial.begin(9600);
  Serial.println("Serial begun");

  Sensors::discover();
  prepareTempValues();

  delay(3000);

  pinMode(8, OUTPUT); // ON
  pinMode(9, OUTPUT); // RESET

  gsm.TurnOn(19200);
  gsm.Echo(0);
}


void loop() {
  if(halted) { delay(100000); return; }

  int reg;
  reg=gsm.CheckRegistration();

  printRegistrationState(reg);

  if(fullyInitialized) {

    if(tempCounter == (SAMPLE_COUNT - 1)) {
      tempCounter = 0;
      for(int i=0;i<SAMPLE_COUNT;i++) {
        Serial.print("Sample #");
        Serial.print(i, DEC);
        Serial.print(": ");
        for(int j=0;j<Sensors::count();j++) {
          Serial.print(tempValues[i][j]);
          Serial.print(" ");
        }
        Serial.println("");
      }
    } else {
      Sensors::initiateRead(750);
      Sensors::readAll(tempValues[tempCounter++]);
      delay(SAMPLE_DELAY - 750);
    }

    // gsm.DumpBuffer();

    // Serial.println("Would send req");
    // if(HTTP::start("internet", "-", "-")) {
    //   Serial.println("HTTP started");

    //   HTTP::post("http://heahdk.net/talk-to-me/bla", "foobar");
    // } else { gsm.DumpBuffer(); }

    halted = true;

  } else if(reg == REG_REGISTERED) {
    gsm.InitParam(PARAM_SET_1);
    fullyInitialized = true;
  }


  delay(INIT_DELAY);
}
