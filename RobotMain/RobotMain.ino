#include <Servo.h>
#include "BTComms.h"
#include "Messages.h"
#include "Drive.h"

enum robotStates{STOP, MOVE};
uint8_t curr_robot_state;

enum robotMoveStates {GOTO_REACTOR, GOTO_STORAGE, GOTO_SUPPLY, AT_REACTOR, AT_STORAGE, AT_SUPPLY};
uint8_t curr_robot_move_state;

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
unsigned long p_time;
bool carrying_spent; // is the fuel rod being carried spent? Default: 0

void setup() {
  Serial.begin(115200);
  Serial.println("Starting robot...");
  msg.setup();
  
  pinMode(upStop, INPUT);
  pinMode(downStop, INPUT);
  pinMode(13, OUTPUT);
  pinMode(29, INPUT_PULLUP);
  crankMotor.attach(4);
  gripperMotor.attach(10);
  
  carrying_spent = 0;
  heartbeat = millis();

  //define starting state//
  fourbarStow();
  gripperOpen();
  curr_robot_state = STOP;
  curr_robot_move_state = AT_REACTOR;
  robotStateMachine();
}

void loop() {
  sendStatus();
  readMsgs();
}

void robotStateMachine(){
  switch(curr_robot_state){
    case STOP:
      base->setLineRaw();
      msg.setMoveStatus(0x01);
      base->stopDriving();
      Serial.println("State: STOP");
    break;
    case MOVE:
      robotMoveStateMachine();
    break;
  }
}

void robotMoveStateMachine(){
  switch(curr_robot_move_state){
    case AT_REACTOR:
      Serial.println("AT_REACTOR");
      curr_robot_move_state = AT_REACTOR;
      gripperOpen();
      fourbarDown();
      gripperClose();
      fourbarStow();
    case GOTO_STORAGE:
      Serial.println("GOTO_STORAGE");
      curr_robot_move_state = GOTO_STORAGE;
      while(!base->lineCrossing()) {
        base->lineFollow(true, 255, true, 255);
      }
      base->stopDriving();
      base->centerVTC();
      base->turnLeft();
      base->stopDriving();
      while(digitalRead(29)){
        base->driveLeft(false, 255);
        base->driveRight(false, 255);
      }
      base->stopDriving();

    case AT_STORAGE: 
      Serial.println("GOTO_STORAGE");
      curr_robot_move_state = AT_STORAGE;
      fourbarExtend();
      gripperOpen();
      fourbarStow();
      while(!base->lineCrossing()) {
        base->lineFollow(true, 255, true, 255);
      }
      base->resetMotors();
      base->centerVTC();
      base->resetMotors();
      base->turnRight();
      base->resetMotors();
      base->turnRight();
    case GOTO_SUPPLY:
      Serial.println("GOTO_SUPPLY");
      curr_robot_move_state = GOTO_SUPPLY;
      //base->turnRight();
      p_time = millis();
      while(digitalRead(29)){
        base->driveLeft(false, 255);
        base->driveRight(false, 255);
        if(millis() - p_time > 5000){
          while(!base->lineCrossing()) {
            base->lineFollow(true, 255, true, 255);
          }
          base->resetMotors();
        }
      }
      base->stopDriving();
    case AT_SUPPLY:
      Serial.println("AT_SUPPLY");
      curr_robot_move_state = AT_SUPPLY;
      gripperOpen();
      fourbarExtend();
      gripperClose();
      fourbarStow();
      while(!base->lineCrossing()) {
        base->lineFollow(true, 255, true, 255);
      }
      base->resetMotors();
      base->centerVTC();
      base->resetMotors();
      base->turnRight();
    case GOTO_REACTOR:
      Serial.println("GOTO_REACTOR");
      curr_robot_move_state = GOTO_REACTOR;
      base->resetMotors();
      while(!base->lineCrossing()) {
        base->lineFollow(true, 255, true, 255);
      }
      base->resetMotors();
      base->turn180();
      base->resetMotors();
      base->stopDriving();
      
      while(digitalRead(29)){
        base->driveLeft(false, 255);
        base->driveRight(false, 255);
      }
      base->stopDriving();
      fourbarDown();
      gripperOpen();
      fourbarStow();
      
    break;
  }
}


/**ARM CODE***/
void fourbarDown(){
  crankMotor.write(50);
  delay(750);
}

void fourbarExtend(){
  crankMotor.write(138);
  delay(750);
}

void fourbarStow(){
  crankMotor.write(150);
  delay(750);
}

void gripperOpen(){
  gripperMotor.write(0);
  delay(750);
}

void gripperClose(){
  gripperMotor.write(180);
  digitalWrite(13, HIGH);
  delay(750);
}


void readMsgs(){
  msg.read();
  if(msg.isStopped()) {
    curr_robot_state = STOP;
    robotStateMachine();
  }

  if(curr_robot_state == STOP && !msg.isStopped()){
    curr_robot_state = MOVE;  
    robotStateMachine();
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

