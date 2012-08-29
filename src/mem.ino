
#include "gsm-stuff.h"

namespace MEM {
  long total = 0;

  void *get(size_t bytes) {
    void *ptr = malloc(bytes);
    if(! ptr) {
      Serial.print("Out of memory!");
    }
  }

  void release(void *ptr) {
#ifdef MEM_DEBUG
    Serial.print("FREE(");
    Serial.print(ptr);
    Serial.println(")");
#endif
    free(ptr);
  }

  void inspect() {
    
  }

}
