#include "Arduino.h"
#include "Messages.h"
#include "BTComms.h"

BTComms comms;

/**
 * Constuctor
 * Initialize everything here when the class is created
 * Note: you cannot call methods that depend on other classes having already been created
 */
Messages::Messages() {
	stopped = false;
  move_status = 0x01;
  open_storage = 0x00;
  open_supply = 0x00;
  stor_rod = 1;
  supply_rod = 4;
}

/**
 * Setup class code that is called from the Arduino sketch setup() function. This doesn't
 * get called until all the other classes have been created.
 */
void Messages::setup() {
	comms.setup();
}


int Messages::openStore(){
  unsigned char stor_rod = ((open_storage << 4) | 0x0F);

  for (int i = 4; i >= 1; i--){
    if ((stor_rod | 0x7F) == 0x7F) return i;
    else stor_rod = ((stor_rod << 1) | 0x0F);
  }
}

int Messages::openSupply(){
  unsigned char supply_rod = (open_supply | 0xF0);

  for (int i = 1; i <= 4; i++){
    if ((supply_rod | 0xFE) == 0xFF) return i;
    else supply_rod = ((supply_rod >> 1) | 0xF0);
  }
}


int Messages::getOpenStorage(){
  return stor_rod;  
}


int Messages::getOpenSupply(){
  return supply_rod;  
}
/**
 * Check if the field is currently in the "stop" state
 * @returns bool value that is true if the robot should be stopped
 */
bool Messages::isStopped() {
	return move_status == 0x01 || move_status == 0x00;
}

void Messages::setMoveStatus(unsigned char stat){
  move_status = stat;
}

unsigned char Messages::getMoveStatus(){
  return move_status;  
}

void Messages::setCurrSupply(unsigned char val){
  supply_availability = val;
}

void Messages::setCurrStorage(unsigned char val){
  storage_availability = val;
}
/**
 * Send a heartbeat message to the field to let it know that your code is alive
 * This should be called by your robot program periodically, say once per second. This
 * timing can easily be done in the loop() function of your program.
 */
void Messages::sendHeartbeat() {
	comms.writeMessage(kHeartbeat, 0x02, 0x00);
}

void Messages::sendRadiationAlert(bool carry_status) {
  comms.writeMessage(carry_status, 0x02, 0x00);  
}

/**
 * Print message for debugging
 * This method prints the message as a string of hex numbers strictly for debugging
 * purposes and is not required to be called for any other purpose.
 */
void Messages::printMessage() {
    for (int i = 0; i < comms.getMessageLength(); i++) {
      Serial.print(comms.getMessageByte(i), HEX);
      Serial.print(" ");
    }
    Serial.println();
}

/**
 * Read messages from the Bluetooth serial connection
 * This method should be called from the loop() function in your arduino code. It will check
 * to see if the lower level comms object has received a complete message, and run the appropriate
 * code to handle the message type. This should just save the state of the message inside this class
 * inside member variables. Then add getters/setters to retrieve the status from your program.
 */
bool Messages::read() {
	if (comms.read()) {
		switch (comms.getMessageByte(0)) {
		case kStorageAvailability:
      setCurrStorage(comms.getMessageByte(5));
			break;
		case kSupplyAvailability:
      setCurrSupply(comms.getMessageByte(5));
			break;
		case kRadiationAlert:
			break;
		case kStopMovement:
      setMoveStatus(0x01);
			break;
		case kResumeMovement:
      setMoveStatus(0x03);
			break;
		case kRobotStatus:
			break;
		case kHeartbeat:
			break;
		}
		return true;
	}
	return false;
}
