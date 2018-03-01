#include <Servo.h>

#include "BTComms.h"
#include "Messages.h"
#include "Drive.h"


enum robotStates {STOP, LINE_FOLLOW, AT_REACTOR, AT_STORAGE, AT_SUPPLY};
uint8_t curr_robot_state = STOP;

enum gripperStates {OPEN, CLOSE, GP_NOTHING};
uint8_t curr_gripper_state;

enum fourBarStates {DOWN, EXTEND, STOW, FB_NOTHING};
uint8_t curr_fourbar_state;

enum atReactorSteps {RT_GRIPPER_OPEN, RT_FOURBAR_DOWN, RT_GRIPPER_CLOSE, RT_FOURBAR_STOW, RT_NOTHING};
uint8_t curr_reactor_step;

enum atSupplySteps {SUP_GRIPPER_OPEN, SUP_FOURBAR_EXTEND, SUP_GRIPPER_CLOSE, SUP_FOURBAR_STOW, SUP_NOTHING};
uint8_t curr_supply_step;

enum atStorageSteps {STOR_FOURBAR_EXTEND, STOR_GRIPPER_OPEN, STOR_FOURBAR_STOW, STOR_NOTHING};
uint8_t curr_storage_step;

Messages msg;
Drive* base = new Drive(6, 7, 20, 21, 53, 5, 47, 49, 2, 3);

unsigned long heartbeat;
unsigned int radiation_count_hb;
unsigned int robot_status;

Servo crankMotor;
Servo gripperMotor;
int upStop = 37;
int downStop = 45;


bool carrying_spent; // is the fuel rod being carried spent? Default: 0

void setup() {
  Serial.begin(115200);
  msg.setup();
  
  crankMotor.attach(4);
  gripperMotor.attach(10);
  pinMode(upStop, INPUT);
  pinMode(downStop, INPUT);

  
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
      //Serial.println("State: LINE_FOLLOW");
    break;  
    case AT_REACTOR:
      switch(curr_reactor_step){
        case RT_GRIPPER_OPEN:
          curr_gripper_state = OPEN;
          gripperStateMachine();
          curr_reactor_step = RT_FOURBAR_DOWN;
        break;
        case RT_FOURBAR_DOWN:
          curr_fourbar_state = DOWN;
          fourBarStateMachine();
          curr_reactor_step = RT_GRIPPER_CLOSE;
        break;
        case RT_GRIPPER_CLOSE:
          curr_gripper_state = CLOSE;
          gripperStateMachine();
          curr_reactor_step = RT_FOURBAR_STOW;
        break;
        case RT_FOURBAR_STOW:
          curr_fourbar_state = STOW;
          fourBarStateMachine();  
          curr_reactor_step = RT_NOTHING;
        break;
        default:
          //do nothing state
          curr_fourbar_state = FB_NOTHING;
          curr_gripper_state = GP_NOTHING; 
           
        break;
      }
    break;
    case AT_SUPPLY:
      switch(curr_reactor_step){
        case SUP_GRIPPER_OPEN:
          curr_gripper_state = OPEN;
          gripperStateMachine();
          curr_reactor_step = SUP_FOURBAR_EXTEND;
        break;
        case SUP_FOURBAR_EXTEND:
          curr_fourbar_state = EXTEND;
          fourBarStateMachine();
          curr_reactor_step = SUP_GRIPPER_CLOSE;
        break;
        case SUP_GRIPPER_CLOSE:
          curr_gripper_state = CLOSE;
          gripperStateMachine();
          curr_reactor_step = SUP_FOURBAR_STOW;
        break;
        case SUP_FOURBAR_STOW:
          curr_fourbar_state = STOW;
          fourBarStateMachine();
          curr_reactor_step = SUP_NOTHING;
        break;
        default:
          //do nothing state
          
        break;
      }
    break;
    case AT_STORAGE: //enum atStorageSteps {STOR_FOURBAR_EXTEND, STOR_GRIPPER_OPEN, STOR_FOURBAR_STOW, STOR_NOTHING};
      switch(curr_storage_step){
        case STOR_FOURBAR_EXTEND:
          
          curr_reactor_step = STOR_GRIPPER_OPEN;
        break;
        case STOR_GRIPPER_OPEN:
          
          curr_reactor_step = STOR_FOURBAR_STOW;
        break;
        case STOR_FOURBAR_STOW:
            
          curr_reactor_step = STOR_NOTHING;
        break;
        default:
          //do nothing state
          
        break;
      }
    break;
  }
  
  msg.read();
  if(msg.isStopped()) curr_robot_state = STOP;
  else if (!msg.isStopped()) curr_robot_state = LINE_FOLLOW;
}

void fourBarStateMachine(){
  switch(curr_fourbar_state){
      case DOWN:
        crankMotor.write(50);
      break;
      case EXTEND:
        crankMotor.write(130);
      break;
      case STOW:
        crankMotor.write(150);
      break;
      default:
        crankMotor.write(150);
      break;
    }
}
void gripperStateMachine(){
  switch(curr_gripper_state){
    case OPEN:
      gripperMotor.write(0);
    break;
    case CLOSE:
      gripperMotor.write(180);
    break;
    default:
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



