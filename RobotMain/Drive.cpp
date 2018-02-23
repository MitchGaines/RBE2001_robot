#include "Drive.h"
#include <Arduino.h>

Drive::Drive(int _l_ln2, int _l_ln1, int _l_enc, int _r_ln2, int _r_ln1, int _r_enc){
    l_ln2 = _l_ln2;
    l_ln1 = _l_ln1;
    l_enc = _l_enc;
    r_ln2 = _r_ln2;
    r_ln1 = _r_ln1;
    r_enc = _r_enc;
}

void Drive::driveForward(){
  
}

Drive::~Drive(){}
