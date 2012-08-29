
#ifndef GSM_STUFF_H
#define GSM_STUFF_H

#include <GSM_Shield.h>

#define SENSORS_PIN 7
#define MAX_SENSORS 20

extern GSM gsm;

namespace HTTP {
  bool start(char *apn, char *user, char *pass);
  bool close(void);
  int get(char *url);
  int startPost(char *url);
  void postData(int len, char *data);
  void finalizePost(void);
}

namespace Sensors {

  void discover(void);
  unsigned int count(void);
  byte *getAddress(unsigned int index);
  float readOne(byte sensorAddress[8]);
  void readAll(float *temps);
  void initiateRead(unsigned int wait);
  char* addressToString(byte addr[8]);

}

#endif
