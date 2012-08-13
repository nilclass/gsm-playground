
#ifndef GSM_STUFF_H
#define GSM_STUFF_H

#include <GSM_Shield.h>

#define SENSORS_PIN 7

extern GSM gsm;

namespace HTTP {
  bool start(char *apn, char *user, char *pass);
  int get(char *url);
  int post(char *url, char *data);
}

namespace Sensors {

  void discover(void);
  unsigned int count(void);
  byte *getAddress(unsigned int index);
  float readOne(byte sensorAddress[8]);
  void readAll(float *temps);
  void initiateRead(void);

}

#endif
