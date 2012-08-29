/* -*- mode:c++ -*- */

#include <OneWire.h>

#include "gsm-stuff.h"


/**
 * A module for DS18x20 temperature sensors.
 **/
namespace Sensors {
  /**
   * Field of sensor addresses.
   **/
  byte sensorAddresses[MAX_SENSORS][8];
  /**
   * Number of sensors found.
   **/
  unsigned int sensorsFound = 0;

  /**
   * The OneWire object used to communicate with sensors.
   **/
  OneWire ow(SENSORS_PIN);

  /**
   * Initiate a read (DS18x20 datasheet calls it 'CONVERT T'), then wait
   * for 'wait' milliseconds. Play around with the value, 750 works well
   * for me.
   * This will broadcast the CONVERT command, so all sensors will comply.
   * You need to call this before calling readOne or readAll.
   **/
  void initiateRead(unsigned int wait) {
    ow.reset(); // reset all devices
    ow.write(0xCC); // SKIP ROM command (~ broadcast)
    ow.write(0x44, 1); // CONVERT T command, keep wire pulled HIGH
    delay(wait); // wait for CONVERT T to finish
  }

  /**
   * Read temperature value of a single sensor, identified by given address.
   **/
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

  /**
   * Read temperature values from all sensors and store them at 'temps'.
   * 'temps' must be a field with at least Sensors::count() elements.
   **/
  void readAll(float *temps) {
    int i;
    for(i=0;i<sensorsFound;i++) {
      temps[i] = readOne(sensorAddresses[i]);
    }
  }

  /**
   * Get a device address, based on it's index in the list.
   * index must be smaller than Sensors::count();
   **/
  byte *getAddress(unsigned int index) {
    return sensorAddresses[index];
  }

  /**
   * Get the number of discovered devices.
   **/
  unsigned int count() {
    return sensorsFound;
  }

  /**
   * Convert device address to human readable hex string
   **/
  char* addressToString(byte addr[8]) {
    static char s[19];
    snprintf(s, 19, "0x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x", addr[0], addr[1], addr[2],
             addr[3], addr[4], addr[5], addr[6], addr[7]);
    return s;
  }

  /**
   * Scan for DS18x20 devices.
   **/
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

