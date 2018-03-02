#include "Drive.h"
#include <Arduino.h>

Drive::Drive(int _l_ln2, int _l_ln1, int _l_inv, int _l_en, int _r_ln2, int _r_ln1, int _r_inv, int _r_en, int _encoder0PinA, int _encoder0PinB){
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
  encoder0PinA = _encoder0PinA;
  encoder0PinB = _encoder0PinB;
  
  encoder0Pos = 0;
  encoder0PinALast = LOW;
  n = LOW;
  driveBackValue = -220;
  turnLeftValue = -190;
  turnRightValue = 220;
  last_high = false;
  
  pinMode(l_ln2, OUTPUT);
  pinMode(l_ln1, OUTPUT);
  pinMode(l_inv, OUTPUT);
  pinMode(l_en, OUTPUT);
  pinMode(r_ln2, OUTPUT);
  pinMode(r_ln1, OUTPUT);
  pinMode(r_inv, OUTPUT);
  pinMode(r_en, OUTPUT);
  pinMode (encoder0PinA, INPUT);
  pinMode (encoder0PinB, INPUT);

}

void Drive::lineFollow(bool right_inv, int base_right_spd, bool left_inv, int base_left_spd){
  float kp = 160;
  float error = 4.86 - linePosition(); //if less than, increase left. if greater than, increase right
  
  int left_speed = base_left_spd + kp*error;
  int right_speed = base_right_spd - kp*error;
  
  driveLeft(left_inv, constrain(left_speed, 0, 255));
  driveRight(right_inv, constrain(right_speed, 0, 255));
}
/**/

void Drive::lineFollowUntil(bool right_inv, int base_right_spd, bool left_inv, int base_left_spd, int desired_loc, int current_loc){
  
  lineFollow(right_inv, base_right_spd, left_inv, base_left_spd);
  
  if(current_loc == desired_loc && lineCrossing()){
    turnRight();
    turnRight();
  }
  if(lineCrossing() && current_loc < desired_loc) current_loc++;
  Serial.print("Current Quitant: ");
  Serial.println(current_loc);
}

void Drive::turn180(){
  driveLeft(true, 255);
  driveRight(false, 255);
}

void Drive::driveLeft(bool inv, int spd){
  if(inv) digitalWrite(l_inv, HIGH);
  digitalWrite(l_en, HIGH);
  analogWrite(l_ln2, spd);
  digitalWrite(l_ln1, LOW);
}

void Drive::driveRight(bool inv, int spd){
  if(inv) digitalWrite(r_inv, HIGH);
  digitalWrite(r_en, HIGH);
  digitalWrite(r_ln2, LOW);
  analogWrite(r_ln1, spd);
}

void Drive::stopDriving(){
  analogWrite(r_en, 0);
  analogWrite(l_en, 0);

  digitalWrite(l_inv, LOW);
  digitalWrite(r_inv, LOW);
  //drive left side stop
  digitalWrite(l_ln2, LOW);
  digitalWrite(l_ln1, LOW);
  //drive right side stop
  digitalWrite(r_ln2, LOW);
  digitalWrite(r_ln1, LOW);
}

float Drive::linePosition(){
  setLineRaw();
  float reading_sum = 0.0;
  float position_sum = 0.0;
  for (int i = 0; i < NUM_SENSORS; i++) {
    reading_sum += sensor_val[i];
    position_sum += (float)sensor_val[i] * (float)(i+1);
  }
  
  return position_sum/reading_sum;
}

bool Drive::lineCrossing(){
  for (int i = 0; i < NUM_SENSORS; i++) {
    if(sensor_val[i]<1000){
      last_high = false;
      return false;
    }
  }
  
  if(!last_high) {
    last_high = true;
    return true;
  }
  return false;
}

void Drive::setLineRaw(){
  
  sensor_val[0] = analogRead(A0);   
  sensor_val[1] = analogRead(A1);   
  sensor_val[2] = analogRead(A2);   
  sensor_val[3] = analogRead(A3);   
  sensor_val[4] = analogRead(A4);   
  sensor_val[5] = analogRead(A5);   
  sensor_val[6] = analogRead(A6);   
  sensor_val[7] = analogRead(A7);  

}
void Drive::centerVTC(){
  encoder0Pos = 0;
  driveLeft(true,255);
  driveRight(true,255);
  while(encoder0Pos > driveBackValue){
    Serial.println(encoder0Pos);

    n = digitalRead(encoder0PinA);
    if ((encoder0PinALast == LOW) && (n == HIGH)) {
      if (digitalRead(encoder0PinB) == LOW) {
        encoder0Pos--;
      } else {
        encoder0Pos++;
      }
      //Serial.println (encoder0Pos);
    
    }
    encoder0PinALast = n;
    if(encoder0Pos == -220){
      stopDriving();
      //encoder0Pos = 0;
    }
  }
}
void Drive::turnLeft(){
  encoder0Pos = 0;
  driveLeft(true,255);
  driveRight(false,255);
  while(encoder0Pos > turnLeftValue){
    n = digitalRead(encoder0PinA);
    if ((encoder0PinALast == LOW) && (n == HIGH)) {
      if (digitalRead(encoder0PinB) == LOW) {
        encoder0Pos--;
      } else {
        encoder0Pos++;
      }
      //Serial.println (encoder0Pos);
    
    }
    encoder0PinALast = n;
    if(encoder0Pos == -190){
      stopDriving();
      encoder0Pos = 0;
    }
  }
}

void Drive::turnRight(){
  encoder0Pos = 0;
  driveLeft(false,255);
  driveRight(true,255);
  while(encoder0Pos < turnRightValue){
    n = digitalRead(encoder0PinA);
    if ((encoder0PinALast == LOW) && (n == HIGH)) {
      if (digitalRead(encoder0PinB) == LOW) {
        encoder0Pos--;
      } else {
        encoder0Pos++;
      }
      //Serial.println (encoder0Pos);
    
    }
    encoder0PinALast = n;
    if(encoder0Pos == 220){
      stopDriving();
      encoder0Pos = 0;
    }
  }
}
Drive::~Drive(){}
