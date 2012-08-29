/* -*- mode:c++ -*- */

#include <stdarg.h>

#include <avr/pgmspace.h>

#include "gsm-stuff.h"

#define INIT_DELAY 3000
// 10 minutes:
#define SAMPLE_DELAY 60000 * 10
#define SAMPLE_COUNT 6

#define CONVERSION_FACTOR 20.0

bool fullyInitialized = false;
GSM gsm;
short *tempValues[SAMPLE_COUNT];
unsigned int tempCounter = 0;

bool halted = false;

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

void *ALLOC(size_t bytes) {
  Serial.print("ALLOC(");
  Serial.print(bytes);
  Serial.println(")");
  void *ptr;
  if(! (ptr = malloc(bytes))) {
    Serial.println("Out of mem, dude!");
    halted = true;
  }
  return ptr;
}

void FREE(void *ptr) {
  Serial.println("FREE()");
  free(ptr);
}

void prepareTempValues() {
  int sensorCount = Sensors::count();

  Serial.print("Sensors:");
  Serial.println(sensorCount);

  for(int i=0;i<SAMPLE_COUNT;i++) {
    tempValues[i] = (short*) ALLOC(sensorCount * sizeof(short));
    if(! tempValues[i]) {
      return;
    }
  }
}

void setup() {

  Serial.begin(9600);
  Serial.println("--RESET--");

  Sensors::discover();
  prepareTempValues();

  delay(3000);

  pinMode(8, OUTPUT); // ON
  pinMode(9, OUTPUT); // RESET

  gsm.TurnOn(19200);
  gsm.Echo(0);
}

#define DEVICE_NAME "test"
#define URL "http://temp-logger.heahdk.net/logger"


void sendSensorData(unsigned int sensorIndex) {
  unsigned int bufSize = (SAMPLE_COUNT * 6) + // sensor data
    2 + strlen(DEVICE_NAME) + // d=device-name
    21 + // &a=0xXXXXXXXXXXXXXXXX 
    3 + // &s=
    1; // \0
  char *buffer = (char*)ALLOC(bufSize);
  byte *addr = Sensors::getAddress(sensorIndex);
  float value;
  
  snprintf(buffer, bufSize, "d=%s&a=%s&s=", DEVICE_NAME, Sensors::addressToString(addr));

  for(int j=0;j<SAMPLE_COUNT;j++) {
    value = ((float) tempValues[j][sensorIndex]) / CONVERSION_FACTOR;
    snprintf(buffer, bufSize, "%s%s%.2d.%.2d",
             buffer,
             j == 0 ? "" : ",",
             (int) floor(value),
             (int) floor((value - floor(value)) * 100));
  }

  Serial.println(buffer);

  HTTP::startPost(URL);
  HTTP::postData(bufSize - 1, buffer);
  HTTP::finalizePost();
  Serial.println("sent.");
  FREE(buffer);
}

void loop() {
  if(halted) { delay(100000); return; }

  int reg;
  reg=gsm.CheckRegistration();

  printRegistrationState(reg);

  if(fullyInitialized) {
    if(tempCounter == SAMPLE_COUNT) {
      tempCounter = 0;

      HTTP::start("internet", "-", "-");

      for(int i=0;i<Sensors::count();i++) {
        sendSensorData(i);
      }

      HTTP::close();

      // halted = true;

    } else {
      Serial.print(".");
      Sensors::initiateRead(750);
      float *floatData = (float*) ALLOC(sizeof(float) * Sensors::count());
      Sensors::readAll(floatData);
      for(int x=0;x<Sensors::count();x++) {
        tempValues[tempCounter][x] = (short) floor(floatData[x] * CONVERSION_FACTOR);
      }
      FREE(floatData);
      delay(SAMPLE_DELAY - 750);
      tempCounter++;
    }

    return;

  } else if(reg == REG_REGISTERED) {
    gsm.InitParam(PARAM_SET_1);
    fullyInitialized = true;
    Serial.println("INIT");
  }


  delay(INIT_DELAY);
}
