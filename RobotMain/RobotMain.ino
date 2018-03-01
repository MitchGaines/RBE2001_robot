#include <Servo.h>
#include "BTComms.h"
#include "Messages.h"
#include "Arm.h"
#include "Drive.h"

enum robotStates {STOP, GOTO_REACTOR, GOTO_STORAGE, GOTO_SUPPLY, AT_REACTOR, AT_STORAGE, AT_SUPPLY};
uint8_t curr_robot_state = STOP;

int starting_quintant = 5;
int current_quintant = 5;
int desired_quintant;

Messages msg;
Drive* base = new Drive(6, 7, 20, 21, 53, 5, 47, 49, 2, 3);
Servo crankMotor, gripperMotor;
const int upStop = 37;
const int downStop = 45;

unsigned long heartbeat;
unsigned int radiation_count_hb;
unsigned int robot_status;

bool carrying_spent; // is the fuel rod being carried spent? Default: 0

void setup() {
  Serial.begin(115200);
  Serial.println("Starting robot...");
  msg.setup();
  
  pinMode(upStop, INPUT);
  pinMode(downStop, INPUT);
  crankMotor.attach(4);
  gripperMotor.attach(10);
  
  carrying_spent = 0;
  heartbeat = millis();

  //define starting state//
  fourbarStow();
  gripperOpen();
}

void loop() {
  sendStatus();
  robotStateMachine();
}

void robotStateMachine(){
  switch(curr_robot_state){
     case STOP:
      msg.setMoveStatus(0x01);
      //base->stopDriving();
      Serial.println("State: STOP");
    break;
    case GOTO_REACTOR:
      base->lineFollowUntil(true, 255, true, 255, desired_quintant, current_quintant); 
      base->turnRight();
      base->turnRight();
      base->driveLeft(false,255);
      base->driveRight(false,255);
      
      if(digitalRead(29)){
        base->stopDriving();
        curr_robot_state = AT_REACTOR;
        robotStateMachine();
      }
    break; 
    case AT_REACTOR:
      gripperOpen();
      fourbarDown();
      gripperClose();
      fourbarStow();

    break;
    case GOTO_SUPPLY:
      /*
      //line follow until hit quintein number
      base->centerVTC();
      base->turnLeft();
      base->driveLeft(false,255);
      base->driveRight(false,255);
      if(digitalRead(29)){
        base->stopDriving();
        curr_robot_state = AT_SUPPLY;
        robotStateMachine();
      }*/
      break;
    case AT_SUPPLY:
      gripperOpen();
      fourbarExtend();
      gripperClose();
      fourbarStow();
      /*
        case SUP_RETURN:
          //INSERT LINE FOLLOW 
          base->centerVTC();
          if((current_quintant-desired_quintant)<0){
            base->turnRight();
          }
          else{
            base->turnLeft();
          }
      }*/
    break;
    case GOTO_STORAGE:
      /*
      //line follow until you hit quientin number
      base->centerVTC();
      base->turnRight();
      base->driveLeft(false,255);
      base->driveRight(false,255);
      if(digitalRead(29)){
        base->stopDriving();
        curr_robot_state = AT_STORAGE;
        robotStateMachine();
      }*/
      break;
    case AT_STORAGE: //enum atStorageSteps {STOR_FOURBAR_EXTEND, STOR_GRIPPER_OPEN, STOR_FOURBAR_STOW, STOR_NOTHING};
      fourbarExtend();
      gripperOpen();
      fourbarStow();
      //base->storeReturn();
      /*
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
      }*/
    break;
  }
  
  msg.read();
  if(msg.isStopped()) curr_robot_state = STOP;
  if(msg.getMoveStatus() == 0x03) curr_robot_state = AT_SUPPLY;
}


/**ARM CODE***/
void fourbarDown(){
  crankMotor.write(50);
  delay(1000);
}

void fourbarExtend(){
  crankMotor.write(138);
  delay(1000);
}

void fourbarStow(){
  crankMotor.write(150);
  delay(1000);
}

void gripperOpen(){
  Serial.println("gripper open");
  gripperMotor.write(0);
  delay(1000);
}

void gripperClose(){
  Serial.println("gripper close");
  gripperMotor.write(180);
  delay(1000);
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



