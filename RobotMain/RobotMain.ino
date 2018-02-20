#include "BTComms.h"
#include "Messages.h"
#include "Drive.h"

Messages msg;
unsigned long heartbeat;
unsigned int radiation_count_hb;
unsigned int robot_status;

bool carrying_spent; // is the fuel rod being carried spent? Default: 0

void setup() {
  carrying_spent = 0;
}

void loop() {
  bluetoothComm();
}



void bluetoothComm() {
  if (millis() > heartbeat) {
    heartbeat = millis() + 1000;
    msg.sendHeartbeat();

    int radiation_status = (carrying_spent) ? 0x2C : 0xFF;
    
    if (!radiation_count_hb) msg.sendRadiationAlert(radiation_status);

    radiation_count_hb = (radiation_count_hb + 1) % 4; 
  }
}
