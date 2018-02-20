#include "BTComms.h"
#include "Messages.h"
#include "Drive.h"

enum robotStates {STOP, DRIVE};
uint8_t curr_robot_state = STOP;

Messages msg;
unsigned long heartbeat;
unsigned int radiation_count_hb;
unsigned int robot_status;

bool carrying_spent; // is the fuel rod being carried spent? Default: 0

void setup() {
  Serial.begin(115200);
  msg.setup();
  carrying_spent = 0;
  heartbeat = millis();
  
  Serial.println("Starting robot...");
}

void loop() {
  sendStatus();
  stateMachine();
  delay(10);
}

void stateMachine(){
  switch(curr_robot_state){
    case STOP:
      msg.setMoveStatus(0x00);
      Serial.println("Robot Stopped");
    break;
    case DRIVE:
      msg.setMoveStatus(0x01);
      Serial.println("Robot Driving");
    break;  
  }
  
  msg.read();
  if(msg.isStopped()) curr_robot_state = STOP;
  else if (!msg.isStopped()) curr_robot_state = DRIVE;
}

void sendStatus() {
  if (millis() > heartbeat) {
    heartbeat = millis() + 1000;
    msg.sendHeartbeat();

    //int radiation_status = 0x2C; //(carrying_spent) ? 0x2C : 0xFF;
    
    //if (!radiation_count_hb) msg.sendRadiationAlert(radiation_status);

    radiation_count_hb = (radiation_count_hb + 1) % 4; 
  }
}
