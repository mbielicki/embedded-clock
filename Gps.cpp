#include "Gps.h"

Gps::Gps(SoftwareSerial* ss, LiquidCrystal_I2C* lcd) {
  this->ss = ss;
  this->lcd = lcd;
  gpsData = ' ';
  charNo = 0;
  timeStr = "";
}

void Gps::processGpsTime(char gpsChar) {
    if (gpsChar == '$') {
      charNo = 0;
      timeStr = "";
    }
    else if (charNo > WAIT_FOR_NEXT_SENTENCE)
      return;

    else if (charNo >= TIME_START_CHAR && charNo < TIME_START_CHAR + TIME_LENGTH)
      if (isDigit(gpsChar))
        timeStr += gpsChar;
      else
        charNo = WAIT_FOR_NEXT_SENTENCE;
    else if (charNo > TIME_START_CHAR + TIME_LENGTH)
      readTimeStr();

    charNo++;
}


void Gps::readTimeStr() {
  hours = timeStr.substring(0, 2).toInt();
  hours += 2; // UTC+2
  minutes = timeStr.substring(2, 4).toInt();
  if (hours >= 0 && hours <= 24 && minutes >= 0 && minutes < 60) {
    success = true;
    lcd->clear();
    lcd->setCursor(4, 0);
    lcd->print("Success");
    lcd->setCursor(4, 1);
    if (hours < 10) lcd->print('0');
    lcd->print(hours);
    lcd->print(':');
    if (minutes < 10) lcd->print('0');
    lcd->print(minutes);
    delay(1000);
    lcd->clear();
  }
}


bool Gps::waitForTime(int maxTime) {
  ss->begin(9600);
  lcd->print("Getting GPS time");

  unsigned long startTime = millis();

  int i = 0;
  while (millis() - startTime < maxTime) {
    if (ss->available()) {
      gpsData = ss->read();
      if (i++ % 16 == 0) {
        lcd->setCursor(0, 1);
        lcd->print(gpsData);
      }
      processGpsTime(gpsData);
      if(success) {
        usTimeStr = "";

        if (hours < 10) 
          usTimeStr += "0";
        usTimeStr += String(hours);

        usTimeStr += ":";

        if (minutes < 10) 
          usTimeStr += "0";
        usTimeStr += String(minutes);

        usTimeStr += ":00";

        return true;
      }
      delay(50);
    }
  }

  return false;
}