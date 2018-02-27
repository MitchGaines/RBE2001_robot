#include "QTRSensors.h"
#include "Drive.h"
#include <Arduino.h>

QTRSensorsRC qtrrc((unsigned char[]) {16, 17, 18, 19, 20, 21, 22}, NUM_SENSORS, TIMEOUT, EMITTER_PIN);

Drive::Drive(int _l_ln2, int _l_ln1, int _l_inv, int _l_en, int _r_ln2, int _r_ln1, int _r_inv, int _r_en){
  qtr_setup = false;
  prev_error = 0;
  
  l_ln2 = _l_ln2;
  l_ln1 = _l_ln1;
  l_inv = _l_inv;
  l_en  = _l_en;
  r_ln2 = _r_ln2;
  r_ln1 = _r_ln1;
  r_inv = _r_inv;
  r_en  = _r_en;
    
  pinMode(l_ln2, OUTPUT);
  pinMode(l_ln1, OUTPUT);
  pinMode(l_inv, OUTPUT);
  pinMode(l_en, OUTPUT);
  pinMode(r_ln2, OUTPUT);
  pinMode(r_ln1, OUTPUT);
  pinMode(r_inv, OUTPUT);
  pinMode(r_en, OUTPUT);

}

void Drive::lineFollow(bool right_inv, int base_right_spd, bool left_inv, int base_left_spd){
  setupQtr();
  int error = line_follow_pos - 1500;
  Serial.print("Error: ");
  Serial.println(error);
  float kp = 50;
  int kd = 0;
  int motor_speed  = kp * error + kd * (error-prev_error);
  prev_error = error;

  int left_speed = base_left_spd - motor_speed;
  int right_speed = base_right_spd + motor_speed;
  
  driveLeft(left_inv, constrain(left_speed, 0, 255));
  driveRight(right_inv, constrain(right_speed, 0, 255));
}

void Drive::driveLeft(bool inv, int spd){
  if(inv) digitalWrite(l_inv, LOW);
  analogWrite(l_en, spd);
  //drive left side 
  digitalWrite(l_ln2, LOW);
  digitalWrite(l_ln1, LOW);
}

void Drive::driveRight(bool inv, int spd){
  if(inv) digitalWrite(r_inv, LOW);
  analogWrite(r_en, spd);
  //drive right side 
  digitalWrite(r_ln2, HIGH);
  digitalWrite(r_ln1, LOW);  
}

void Drive::stopDriving(){
  analogWrite(r_en, 0);
  analogWrite(l_en, 0);
  
  //drive left side stop
  digitalWrite(l_ln2, HIGH);
  digitalWrite(l_ln1, LOW);
  //drive right side stop
  digitalWrite(r_ln2, LOW);
  digitalWrite(r_ln1, LOW);
}


void Drive::displayQtr(){
  line_follow_pos = qtrrc.readLine(sensorValues);
  Serial.println(line_follow_pos);
  delay(250);
}

void Drive::setupQtr(){
  if(qtr_setup){
    displayQtr();
  } else {
    Serial.println("Calibrating line follower. . .");
    for (int i = 0; i < 400; i++) {
      qtrrc.calibrate();       // reads all sensors 10 times at 2500 us per read (i.e. ~25 ms per call)
    }
    qtr_setup = true;
    delay(1000);
    displayQtr();
  }
}


Drive::~Drive(){}
