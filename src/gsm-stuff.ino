
#include <GSM_Shield.h>

GSM gsm;

void setup() {

  Serial.begin(9600);

  gsm.TurnOn(9600);
  gsm.InitParam(PARAM_SET_1);
  gsm.Echo(1);

}

void loop() {

  int reg;
  reg=gsm.CheckRegistration();
  switch (reg){    
  case REG_NOT_REGISTERED:
    Serial.println("not registered");
    gsm.SendATCmdWaitResp("AT+CPIN=\"1234\"", 1000, 50, "OK", 1);
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
  delay(2000);


  Serial.print("registered: ");
  Serial.println(gsm.IsRegistered());

  delay(5000);
}
