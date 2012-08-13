/* -*- mode:c++ -*- */

#include <OneWire.h>

#include "gsm-stuff.h"


/* float *tempValues[60]; */
/* unsigned int tempCounter = 0; */

namespace Sensors {
  byte sensorAddresses[4][8];
  unsigned int sensorsFound = 0;
  OneWire ow(SENSORS_PIN);

  void initiateRead(unsigned int wait) {
    ow.reset(); // reset all devices
    ow.write(0xCC); // SKIP ROM command (~ broadcast)
    ow.write(0x44, 1); // CONVERT T command, keep wire pulled HIGH
    delay(wait); // wait for CONVERT T to finish
  }

  float readOne(byte sensorAddress[8]) {
    int i;
    byte data[8];
    byte checksum;

    ow.reset();

    ow.select(sensorAddress); // select device
    ow.write(0xBE); // READ SCRATCHPAD command

    for(i=0;i<8;i++) {
      data[i] = ow.read();
    }

    checksum = ow.read(); // 9th byte is checksum

    if(ow.crc8(data, 8) == checksum) {
      return ((data[1] << 8) + data[0]) * 0.0625;
    } else {
      return -1; // CRC mismatch
    }
  }

  void readAll(float *temps) {
    int i;
    for(i=0;i<sensorsFound;i++) {
      temps[i] = readOne(sensorAddresses[i]);
    }
  }

  byte *getAddress(unsigned int index) {
    return sensorAddresses[index];
  }

  unsigned int count() {
    return sensorsFound;
  }

  char* addressToString(byte addr[8]) {
    static char s[19];
    snprintf(s, 19, "0x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x", addr[0], addr[1], addr[2],
             addr[3], addr[4], addr[5], addr[6], addr[7]);
    return s;
  }

  void discover() {
    byte addr[8];
    sensorsFound = 0;
    while(ow.search(addr)) {

      if(OneWire::crc8(addr, 7) != addr[7]) {
        //Serial.println("CRC check failed!");
        continue;
      }

      if(addr[0] != 0x10 && addr[0] != 0x28) {
        //Serial.println("Device not recognized!");
        continue;
      }

      // Serial.print("Found device: ");
      // Serial.println(addressToString(addr));
      memcpy(sensorAddresses[sensorsFound], addr, 8);
      sensorsFound++;
    }

    // Serial.println("No more devices.");

    ow.reset_search();
  }
}

