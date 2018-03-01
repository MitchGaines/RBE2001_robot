#include <Servo.h>
#include "BTComms.h"
#include "Messages.h"
#include "Drive.h"

enum robotStates {STOP, GOTO_REACTOR, GOTO_STORAGE, GOTO_SUPPLY, AT_REACTOR, AT_STORAGE, AT_SUPPLY};
uint8_t curr_robot_state = STOP;

enum gripperStates {OPEN, CLOSE, GP_NOTHING};
uint8_t curr_gripper_state;

enum fourBarStates {DOWN, EXTEND, STOW, FB_NOTHING};
uint8_t curr_fourbar_state;

enum atReactorSteps {RT_GRIPPER_OPEN, RT_FOURBAR_DOWN, RT_GRIPPER_CLOSE, RT_FOURBAR_STOW, RT_NOTHING};
uint8_t curr_reactor_step;

enum atSupplySteps {SUP_GRIPPER_OPEN, SUP_FOURBAR_EXTEND, SUP_GRIPPER_CLOSE, SUP_FOURBAR_STOW, SUP_RETURN, SUP_NOTHING};
uint8_t curr_supply_step;

enum atStorageSteps {STOR_FOURBAR_EXTEND, STOR_GRIPPER_OPEN, STOR_FOURBAR_STOW, STOR_RETURN, STOR_NOTHING};
uint8_t curr_storage_step;

int starting_quintant = 1;
int current_quintant = 1;
int desired_quintant;

Messages msg;
Drive* base = new Drive(6, 7, 20, 21, 53, 5, 47, 49, 2, 3);

const int upStop = 37;
const int downStop = 45;

unsigned long heartbeat;
unsigned int radiation_count_hb;
unsigned int robot_status;
Servo crankMotor;
Servo gripperMotor;



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
     case STOP:
      msg.setMoveStatus(0x01);
      base->stopDriving();
      Serial.println("State: STOP");
    break;
    case GOTO_REACTOR:
      base->lineFollow(true, 255, true, 255); 
      if(current_quintant == 5 && base->lineCrossing()){
        curr_robot_state = STOP;
        robotStateMachine();
      }
      if(base->lineCrossing() && current_quintant < 5) current_quintant++;
      Serial.print("Current Quitant: ");
      Serial.println(current_quintant);
      //Serial.println("State: APPROACH_REACTOR");
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
          //state ends with stowed closed gripper at reactor 
        break;
        default:
          //do nothing state
          curr_fourbar_state = FB_NOTHING;
          curr_gripper_state = GP_NOTHING; 
           
        break;
      }
    break;
    case GOTO_SUPPLY:
      //line follow until hit quintein number
      base->centerVTC();
      base->turnLeft();
      base->driveLeft(false,255);
      base->driveRight(false,255);
      if(digitalRead(29)){
        base->stopDriving();
        curr_robot_state = AT_SUPPLY;
        robotStateMachine();
      }
      break;
    case AT_SUPPLY:
      switch(curr_supply_step){
        case SUP_GRIPPER_OPEN:
          curr_gripper_state = OPEN;
          gripperStateMachine();
          curr_supply_step = SUP_FOURBAR_EXTEND;
        break;
        case SUP_FOURBAR_EXTEND:
          curr_fourbar_state = EXTEND;
          fourBarStateMachine();
          curr_supply_step = SUP_GRIPPER_CLOSE;
        break;
        case SUP_GRIPPER_CLOSE:
          curr_gripper_state = CLOSE;
          gripperStateMachine();
          curr_supply_step = SUP_FOURBAR_STOW;
        break;
        case SUP_FOURBAR_STOW:
          curr_fourbar_state = STOW;
          fourBarStateMachine();
          curr_supply_step = SUP_RETURN;
          //state ends with closed stowed gripper at supply 
        break;
        case SUP_RETURN:
          //INSERT LINE FOLLOW 
          base->centerVTC();
          if((current_quintant-desired_quintant)<0){
            base->turnRight();
          }
          else{
            base->turnLeft();
          }
          curr_supply_step = SUP_NOTHING;
        default:
          //do nothing state
          
        break;

      }
    break;
    case GOTO_STORAGE:
      //line follow until you hit quientin number
      base->centerVTC();
      base->turnRight();
      base->driveLeft(false,255);
      base->driveRight(false,255);
      if(digitalRead(29)){
        base->stopDriving();
        curr_robot_state = AT_STORAGE;
        robotStateMachine();
      }
      break;
    case AT_STORAGE: //enum atStorageSteps {STOR_FOURBAR_EXTEND, STOR_GRIPPER_OPEN, STOR_FOURBAR_STOW, STOR_NOTHING};
      switch(curr_storage_step){
        case STOR_FOURBAR_EXTEND:
          curr_fourbar_state = EXTEND;
          fourBarStateMachine();
          curr_storage_step = STOR_GRIPPER_OPEN;
        break;
        case STOR_GRIPPER_OPEN:
          curr_gripper_state = OPEN;
          gripperStateMachine();
          curr_storage_step = STOR_FOURBAR_STOW;
        break;
        case STOR_FOURBAR_STOW:
          curr_fourbar_state = STOW;
          fourBarStateMachine();  
          curr_storage_step = STOR_RETURN;
          //currently at storage with stowed open gripper 
        break;
        case STOR_RETURN:
          //INSERT LINE CROSSING
          base->centerVTC();
          if((current_quintant-desired_quintant)<0){
            base->turnLeft();
          }
          else{
            base->turnRight();
          }
          curr_storage_step = STOR_NOTHING;
          //state ends with robot at center line with stowed open gripper
        break;
        default:
          //do nothing state
          
        break;
      }
    break;
  }
  
  msg.read();
  if(msg.isStopped()) curr_robot_state = STOP;
  if(msg.getMoveStatus() == 0x03) curr_robot_state = GOTO_REACTOR;
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



