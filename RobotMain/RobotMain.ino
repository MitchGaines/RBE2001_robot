#include "BTComms.h"
#include "Messages.h"
#include "Drive.h"

enum robotStates {STOP, LINE_FOLLOW};
uint8_t curr_robot_state = LINE_FOLLOW;

enum gripperStates {OPEN, CLOSE};
uint8_t curr_gripper_state;

enum fourBarStates {DOWN, EXTEND, STOW};
uint8_t curr_fourbar_state;

enum atReactorSteps {GRIPPER_OPEN, FOURBAR_DOWN, GRIPPER_CLOSE, FOURBAR_STOW};
uint8_t curr_reactor_step;

enum atSupplySteps {GRIPPER_OPEN, FOURBAR_EXTEND, GRIPPER_CLOSE, FOURBAR_STOW};
uint8_t curr_supply_step;

enum atStorageSteps {FOURBAR_EXTEND, GRIPPER_OPEN, FOURBAR_STOW};
uint8_t curr_storage_step;

Messages msg;
Drive* base = new Drive(6, 7, 37, 43, 8, 9, 39, 24);

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
      base->stopDriving();
      Serial.println("State: STOP");
    break;
    case LINE_FOLLOW:
      base->lineFollow(true, 255, true, 255); 
      Serial.println("State: LINE_FOLLOW");
    break;  
    case AT_REACTOR:
      fourBarStateMachine();
      
      
    break;
    case AT_SUPPLY:

    break;
    case AT_STORAGE:

    break;
  }
  
  msg.read();
  if(msg.isStopped()) curr_robot_state = STOP;
  else if (!msg.isStopped()) curr_robot_state = LINE_FOLLOW;
}

void fourBarStateMachine(){
  switch(curr_fourbar_state){
      case DOWN:
      break;
      case EXTEND:
      break;
      case STOW:
      break;
    }
}

void gripperStateMachine(){
  switch(curr_gripper_state){
    case OPEN:
    break;
    case CLOSE:
    break;
  }
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
