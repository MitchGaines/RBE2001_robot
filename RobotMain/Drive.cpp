#include "QTRSensors.h"
#include "PID_v1.h"
#include "Drive.h"
#include <Arduino.h>

QTRSensorsRC qtrrc((unsigned char[]) {16, 17, 18, 19, 20, 21, 22}, NUM_SENSORS, TIMEOUT, EMITTER_PIN);

Drive::Drive(int _l_ln2, int _l_ln1, int _l_inv, int _l_en, int _r_ln2, int _r_ln1, int _r_inv, int _r_en){
  qtr_setup = false;
  
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
  pinMode(r_ln2, OUTPUT);
  pinMode(r_ln1, OUTPUT);
  pinMode(r_inv, OUTPUT);
}

void Drive::drive(bool right_inv, int right_speed, bool left_inv, int left_speed){
  driveLeft(left_inv, left_speed);
  driveRight(right_inv, right_speed);
}

void Drive::driveLeft(bool inv, int spd){
  if(inv)digitalWrite(l_inv, HIGH);
  else digitalWrite(l_inv, LOW);
  analogWrite(l_en, spd);
  //drive left side 
  digitalWrite(l_ln2, LOW);
  digitalWrite(l_ln1, LOW);
}

void Drive::driveRight(bool inv, int spd){
  if(inv)digitalWrite(r_inv, HIGH);
  else digitalWrite(r_inv, LOW);
  analogWrite(r_en, spd);
  //drive right side 
  digitalWrite(r_ln2, HIGH);
  digitalWrite(r_ln1, LOW);  
}

void Drive::stopDriving(){
  digitalWrite(r_inv, LOW);
  digitalWrite(l_inv, HIGH);
  
  //drive left side stop
  digitalWrite(l_ln2, HIGH);
  digitalWrite(l_ln1, LOW);
  //drive right side stop
  digitalWrite(r_ln2, LOW);
  digitalWrite(r_ln1, LOW);
}


void Drive::displayQtr(){
  unsigned int position = qtrrc.readLine(sensorValues);
  Serial.println(position); // comment this line out if you are using raw values

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
