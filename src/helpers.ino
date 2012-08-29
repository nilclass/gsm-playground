
#include <stdio.h>
#include <stdarg.h>
#include "helpers.h"

void debug_msg(char *format, ...) {

#ifdef DEBUG

  char buffer[DEBUG_BUF_SIZE];
  va_list ap;
  va_start(ap, format);
  vsnprintf(buffer, DEBUG_BUF_SIZE, format, ap);
  va_end(ap);
  Serial.print("DEBUG: ");
  Serial.println(buffer);

#endif

}
