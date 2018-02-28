#include "QTRSensors.h"
#include "Drive.h"
#include <Arduino.h>
#include <Wire.h>

Drive::Drive(int _l_ln2, int _l_ln1, int _l_inv, int _l_en, int _r_ln2, int _r_ln1, int _r_inv, int _r_en){
  Wire.begin();
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
  for(int i = 0; i < NUM_SENSORS; i++){
    Serial.print(' ');
    Serial.print(sensor_val[i]);  
  }
  Serial.print(" | ");
  Serial.print(linePosition());
  float kp = 10;
  float error = 4.86 - linePosition(); //if less than, increase left. if greater than, increase right
  
  int left_speed = base_left_spd - kp*error;
  int right_speed = base_right_spd + kp*error;
  Serial.print(" -- error: ");
  Serial.print(error);
  
  
  
  driveLeft(left_inv, constrain(left_speed, 0, 255));
  driveRight(right_inv, constrain(right_speed, 0, 255));
}

void Drive::driveLeft(bool inv, int spd){
  if(inv) digitalWrite(l_inv, HIGH);
  Serial.print(" --- L: ");
  Serial.print(spd);
  digitalWrite(l_en, HIGH);
  analogWrite(l_ln2, spd);
  digitalWrite(l_ln1, LOW);
}

void Drive::driveRight(bool inv, int spd){
  if(inv) digitalWrite(r_inv, HIGH);
  Serial.print(" | R: ");
  Serial.println(spd);
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

Drive::~Drive(){}
