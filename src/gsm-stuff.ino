#include <stdarg.h>

#include <avr/pgmspace.h>

#include "gsm-stuff.h"

bool haveSwitchedParams = false;
bool haveSentSMS = false;
GSM gsm;

/* #include <OneWire.h> */
/* #define OW_PIN 7 */

/* OneWire ow(OW_PIN); */

/* byte sensorAddresses[4][8]; */
/* unsigned int sensorsFound = 0; */

/* float *tempValues[60]; */
/* unsigned int tempCounter = 0; */

/* char* addressToString(byte addr[8]) { */
/*   static char s[19]; */
/*   snprintf(s, 19, "0x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x", addr[0], addr[1], addr[2], */
/*            addr[3], addr[4], addr[5], addr[6], addr[7]); */
/*   return s; */
/* } */

/* void findSensors() { */
/*   byte addr[8]; */
/*   sensorsFound = 0; */
/*   while(ow.search(addr)) { */

/*     if(OneWire::crc8(addr, 7) != addr[7]) { */
/*       Serial.println("CRC check failed!"); */
/*       continue; */
/*     } */

/*     if(addr[0] != 0x10 && addr[0] != 0x28) { */
/*       Serial.println("Device not recognized!"); */
/*       continue; */
/*     } */

/*     Serial.print("Found device: "); */
/*     Serial.println(addressToString(addr)); */
/*     memcpy(sensorAddresses[sensorsFound], addr, 8); */
/*     sensorsFound++; */
/*   } */

/*   Serial.println("No more devices."); */

/*   ow.reset_search(); */
/* } */

/* void initiateRead() { */
/*   ow.reset(); // reset all devices */
/*   ow.write(0xCC); // SKIP ROM command (~ broadcast) */
/*   ow.write(0x44, 1); // CONVERT T command, keep wire pulled HIGH */
/*   delay(750); // wait for CONVERT T to finish */
/* } */


/* float readTemperature(byte address[8]) { */
/*   int i; */
/*   byte data[8]; */
/*   byte checksum; */

/*   ow.reset(); */

/*   ow.select(address); // select device */
/*   ow.write(0xBE); // READ SCRATCHPAD command */

/*   for(i=0;i<8;i++) { */
/*     data[i] = ow.read(); */
/*   } */

/*   checksum = ow.read(); // 9th byte is checksum */

/*   if(ow.crc8(data, 8) == checksum) { */
/*     return ((data[1] << 8) + data[0]) * 0.0625; */
/*   } else { */
/*     return -1; // CRC mismatch */
/*   } */
/* } */

/* void readTemperatures() { */
/*   int i; */
/*   float temps[sensorsFound - 1]; */
/*   for(i=0;i<sensorsFound;i++) { */
/*     temps[i] = readTemperature(sensorAddresses[i]); */
/*     /\* Serial.print("Sensor #"); *\/ */
/*     /\* Serial.print(i); *\/ */
/*     /\* Serial.print(": "); *\/ */
/*     /\* if(temp == -1) { *\/ */
/*     /\*   Serial.println("CRC mismatch"); *\/ */
/*     /\* } else { *\/ */
/*     /\*   Serial.println(temp); *\/ */
/*     /\* } *\/ */
/*   } */
/*   tempValues[tempCounter++] = temps; */
/* } */

bool stop = false;

void mallocFailed() {
  Serial.println("No memory, dude!");
  stop = true;
}


/* bool check_command(char *command, char *expected_result) { */
/*   if(AT_RESP_OK == gsm.SendATCmdWaitResp(command, 500, 100, expected_result, 5)) {   */
/*     return true; */
/*   } else { */
/*     Serial.println("Result didn't match expectation."); */
/*     return false; */
/*   } */
/* } */

/* #define CHECK(cmd, exp) { if(check_command(cmd, exp) != true) { gsm.DumpBuffer(); stop = true; return; } gsm.DumpBuffer(); } */
/* #define CHECK_OK(cmd) CHECK(cmd, "OK") */

void setup() {

  Serial.begin(9600);
  Serial.println("Serial begun");

  // findSensors();

  // for(int i=0;i<60;i++) {
  //   if(! (tempValues[i] = (float*) malloc(sensorsFound * sizeof(float)))) {
  //     mallocFailed();
  //     return;
  //   }
  // }

  delay(3000);

  pinMode(8, OUTPUT); // ON
  pinMode(9, OUTPUT); // RESET

  gsm.TurnOn(19200);
  //gsm.TurnOn(115000);
  gsm.Echo(0);
}

void loop() {
  if(stop) { return; }

  /* /\* Serial.print("initialized: "); *\/ */
  /* /\* Serial.println(gsm.IsInitialized()); *\/ */

  int reg;
  reg=gsm.CheckRegistration();
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
    break;  }
  
  /* Serial.print("registered: "); */
  /* Serial.println(gsm.IsRegistered()); */

  if(haveSwitchedParams) {

    gsm.DumpBuffer();


    Serial.println("Would send req");
    if(HTTP::start("internet", "-", "-")) {
      Serial.println("HTTP started");
    
      HTTP::post("http://heahdk.net/talk-to-me/bla", "foobar");
    } else { gsm.DumpBuffer(); }

    stop = true;

    // initiateRead();
    // readTemperatures();

    // delay(250);

    // if(tempCounter >= 60) {
    //   tempCounter = 0;
    //   for(int i=0;i<60;i++) {
    //     Serial.print("DATASET ");
    //     Serial.print(i, DEC);
    //     Serial.println(":");
    //     for(int j=0;j<sensorsFound;j++) {
    //       Serial.print(" - ");
    //       Serial.println(tempValues[i][j]);
    //     }
    //   }
    // }


  /*   /\* StartGPRS("internet", "-", "-"); *\/ */

  /*   /\* POSTRequest("http://heahdk.net/talk-to-me/huhu"); *\/ */

  /*   /\* gsm.DumpBuffer(); *\/ */

    /* stop = true; */

  /*   /\* if(! haveSentSMS) { *\/ */
  /*   /\*   gsm.SendSMS("+491624484552", "Hello World!"); *\/ */
  /*   /\*   haveSentSMS = true; *\/ */
  /*   /\* } *\/ */
  } else if(reg == REG_REGISTERED) {
    gsm.InitParam(PARAM_SET_1);
    haveSwitchedParams = true;
  }


  delay(3000);

}
