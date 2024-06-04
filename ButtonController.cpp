#include "ButtonController.h"
#include <Arduino.h>

ButtonController::ButtonController(int setPin, int incPin, int decPin, int cycleTime) {
    this->setPin = setPin;
    this->incPin = incPin;
    this->decPin = decPin;
    this->cyclesPerSec = 1000 / cycleTime;
    this->cyclesToActivateSetting = 1;
  }

void ButtonController::processInput() {
  setPressed = (digitalRead(setPin) == 0) ? setPressed + 1 : 0 ;
  incPressed = (digitalRead(incPin) == 0) ? incPressed + 1 : 0 ;
  decPressed = (digitalRead(decPin) == 0) ? decPressed + 1 : 0 ;
}

void ButtonController::begin() {
  pinMode(setPin, INPUT_PULLUP);
  pinMode(incPin, INPUT_PULLUP);
  pinMode(decPin, INPUT_PULLUP);
}


void ButtonController::listenForStop(int delayMillis) {
  if (setPressed + incPressed + decPressed > 0)
    alarmJustCancelled = true;
  delay(delayMillis);
}

bool ButtonController::stateChanged() {
  if (setPressed > cyclesToActivateSetting) {
    setPressed = -5 * cyclesToActivateSetting;
    return true;
  }
  return false;
}

bool ButtonController::triggerInc() {
  if (incPressed == 1 || incPressed > cyclesPerSec && incPressed % (cyclesPerSec / 2) == 0)
    return true;
  return false;
}

bool ButtonController::triggerDec() {
  if (decPressed == 1 || decPressed > cyclesPerSec && decPressed % (cyclesPerSec / 2) == 0)
    return true;
  return false;
}