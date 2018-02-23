#include "BTComms.h"
#include "Messages.h"
#include "Drive.h"

enum robotStates {STOP, AUTO};
uint8_t curr_robot_state = STOP;

enum gripperStates {OPEN, CLOSE};
uint8_t curr_gripper_state = OPEN;

enum fourBarStates {DOWN, EXTEND, STOW};
uint8_t curr_fourbar_state = STOW;

Messages msg;
Drive* base = new Drive(5, 6, 7, 8, 9, 10);

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
  robotStateMachine();
}

void robotStateMachine(){
  switch(curr_robot_state){
    case STOP: // don't do anything if in this state
      Serial.println("Robot Stopped");
    break;
    case AUTO:
      base->driveForward();
      Serial.println("Robot Driving Autonomously");
    break;  
  }
  
  msg.read();
  if(msg.isStopped()) curr_robot_state = STOP;
  else if (!msg.isStopped()) curr_robot_state = AUTO;
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
