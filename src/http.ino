#include "gsm-stuff.h"

void at_command(char *format, ...) {
  char at_command[256];

  va_list ap;
  va_start(ap, format);
  gsm.SendData("AT+");
  vsnprintf(at_command, 256, format, ap);

  /* Serial.print("AT COMMAND: "); */
  /* Serial.println(at_command); */
  
  va_end(ap);
  gsm.SendData(at_command);
  gsm.SendData("\r\n");
  //gsm.DumpBuffer();
}

namespace HTTP {

  /**
   ** private
   **/

  bool SAPBR(int cmd, int cid, char *key, char *value) {
    char at_command[128];
    snprintf(at_command, 128, "AT+SAPBR=%d,%d,\"%s\",\"%s\"", cmd, cid, key, value);
    return(AT_RESP_OK == gsm.SendATCmdWaitResp(at_command, 500, 100, "OK", 5));
  }

  bool SAPBR(int cmd, int cid) {
    char at_command[128];
    snprintf(at_command, 128, "AT+SAPBR=%d,%d", cmd, cid);
    return(AT_RESP_OK == gsm.SendATCmdWaitResp(at_command, 500, 100, "OK", 5));
  }

  bool AT_HTTP(char *cmd) {
    char at_command[20];
    snprintf(at_command, 20, "AT+HTTP%s", cmd);
    return(AT_RESP_OK == gsm.SendATCmdWaitResp(at_command, 500, 100, "OK", 5));  
  }

  bool AT_HTTP(char *cmd, int action) {
    char at_command[20];
    snprintf(at_command, 20, "AT+HTTP%s=%d", cmd, action);
    return(AT_RESP_OK == gsm.SendATCmdWaitResp(at_command, 500, 100, "OK", 5));  
  }

  bool AT_HTTP(char *cmd, char *k, char* v) {
    char at_command[64];
    snprintf(at_command, 64, "AT+HTTP%s=\"%s\",\"%s\"", cmd, k, v);
    return(AT_RESP_OK == gsm.SendATCmdWaitResp(at_command, 500, 100, "OK", 5));  
  }

  bool AT_HTTPDATA(int size, int time) {
    char at_command[64];
    snprintf(at_command, 64, "AT+HTTPDATA=%d,%d", size, time);
    return(AT_RESP_OK == gsm.SendATCmdWaitResp(at_command, 500, 100, "DOWNLOAD", 1));
  }

  bool StartGPRS(char *apn, char *user, char *pass) {
    SAPBR(2, 1);
    gsm.DumpBuffer();
    char *respStart = gsm.GetRespAt("+SAPBR");
    if(! respStart) {
      Serial.println("ERROR: +SAPBR response not found");
      return false;
    }
    int cid, status;
    sscanf(respStart, "+SAPBR: %d,%d", &cid, &status);
    if(status == 1) {
      // is connected
      return true;
    }
    SAPBR(3, 1, "CONTYPE", "GPRS");
    gsm.DumpBuffer();
    SAPBR(3, 1, "APN", apn); 
    gsm.DumpBuffer();
    SAPBR(3, 1, "USER", user); 
    gsm.DumpBuffer();
    SAPBR(3, 1, "PWD", pass); 
    gsm.DumpBuffer();
    return SAPBR(1, 1);
  }

  void PrepareRequest(char *url) {
    AT_HTTP("INIT");
    gsm.DumpBuffer();
    AT_HTTP("PARA", "CID", "1");
    gsm.DumpBuffer();
    AT_HTTP("PARA", "URL", url);
    gsm.DumpBuffer();
  }

  /**
   ** public
   **/

  // initialize HTTP, connect to GPRS and stuff
  bool start(char *apn, char *user, char *pass) {
    // TODO: initiate multi-request stuff, if needed.
    return StartGPRS(apn, user, pass);
  }

  bool close() {
    return SAPBR(0, 1);
  }

  int get(char *url) {
    PrepareRequest(url);

    AT_HTTP("ACTION", 0);
    gsm.DumpBuffer();
  }

  int startPost(char *url) {
    PrepareRequest(url);
  }

  void postData(int len, char *data) {
    AT_HTTPDATA(len, 1000);
    for(int i=0;i<len;i++) {
      gsm.SendData(data[i]);
    }
    gsm.EOL();
    gsm.WaitResp(500, 50);
    gsm.DumpBuffer();
  }

  void finalizePost() {
    AT_HTTP("ACTION", 1);
    gsm.WaitResp(500, 50);
    gsm.DumpBuffer();
    AT_HTTP("TERM");
    gsm.DumpBuffer();
  }

};
