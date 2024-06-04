#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <Arduino.h>
#include "pitches.h"
#define BUZZER 3
#define ALARM_ADDRESS 0 // EEPROM address to store alarm time

enum State {
  NORMAL, SETTING_H, SETTING_M
};

class AlarmSetter {
  LiquidCrystal_I2C* lcd;
  int blinkingCounter = 0;

public:
  State state = NORMAL;
  int alarmHour = 6;
  int alarmMinute = 0;

  AlarmSetter(LiquidCrystal_I2C* lcd) {
    this->lcd = lcd;
  }

  void changeState() {
    lcd->clear();
    saveAlarmToEEPROM();
    if (state == NORMAL) state = SETTING_H;
    else if (state == SETTING_H) state = SETTING_M;
    else if (state == SETTING_M) state = NORMAL;

    
    tone(BUZZER, NOTE_C4, 400);
  }

  void inc() {
    if (state == SETTING_H) {
      alarmHour++;
      if (alarmHour > 23) alarmHour = 0;
    }
    if (state == SETTING_M) {
      alarmMinute++;
      if (alarmMinute > 59) alarmMinute = 0;
    }
  }

  void dec() {
    if (state == SETTING_H) {
      alarmHour--;
      if (alarmHour < 0) alarmHour = 23;
    }
    if (state == SETTING_M) {
      alarmMinute--;
      if (alarmMinute < 0) alarmMinute = 59;
    }
  }

  void display() {

    lcd->setCursor(8, 0);
    if (state == SETTING_H && blinks()) {
      lcd->print("  ");
    } else {
      if (alarmHour < 10) lcd->print('0');
      lcd->print(alarmHour);
    }

    lcd->print(":");

    if (state == SETTING_M && blinks()) {
      lcd->print("  ");
    } else {
      if (alarmMinute < 10) lcd->print('0');
      lcd->print(alarmMinute);
    }
  }

  bool blinks() {
    blinkingCounter++;
    return blinkingCounter % 6 > 2;
  }


    
  void loadAlarmFromEEPROM()
  {
    // Read alarm hour from EEPROM
    int storedHour = EEPROM.read(ALARM_ADDRESS);
    if (storedHour >= 0 && storedHour < 24)
    {
      alarmHour = storedHour;
    }

    // Read alarm minute from EEPROM
    int storedMinute = EEPROM.read(ALARM_ADDRESS + 1);
    if (storedMinute >= 0 && storedMinute < 60)
    {
      alarmMinute = storedMinute;
    }
  }

  void saveAlarmToEEPROM()
  {
    EEPROM.write(ALARM_ADDRESS, alarmHour);
    EEPROM.write(ALARM_ADDRESS + 1, alarmMinute);
  }

  void setAlarmTime(int hour, int minute)
  {
    alarmHour = hour;
    alarmMinute = minute;
    saveAlarmToEEPROM();
  }
};