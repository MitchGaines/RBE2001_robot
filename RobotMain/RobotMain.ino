#include "BTComms.h"
#include "Messages.h"
#include "Drive.h"

Messages msg;
unsigned long heartbeat;
unsigned int radiation_count_hb;
unsigned int robot_status;

bool carrying_spent; // is the fuel rod being carried spent? Default: 0

void setup() {
  msg.setup();
  carrying_spent = 0;
  heartbeat = millis();
}

void loop() {
  bluetoothComm();
}



void bluetoothComm() {
  if (millis() > heartbeat) {
    heartbeat = millis() + 1000;
    msg.sendHeartbeat();

    int radiation_status = 0x2C; //(carrying_spent) ? 0x2C : 0xFF;
    
    if (!radiation_count_hb) msg.sendRadiationAlert(radiation_status);

    radiation_count_hb = (radiation_count_hb + 1) % 4; 
  }
}
