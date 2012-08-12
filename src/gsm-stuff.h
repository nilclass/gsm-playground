
#ifndef GSM_STUFF_H
#define GSM_STUFF_H

#include <GSM_Shield.h>

extern GSM gsm;

namespace HTTP {
  bool start(char *apn, char *user, char *pass);
  int get(char *url);
  int post(char *url, char *data);
}

#endif
