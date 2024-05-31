#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

#define TIME_START_CHAR 7
#define TIME_LENGTH 5
#define WAIT_FOR_NEXT_SENTENCE 100

class Gps {
  int charNo = 0;
  char gpsData = ' ';
  String timeStr = "";
  SoftwareSerial* ss;
  LiquidCrystal_I2C* lcd;
  void processGpsTime(char gpsChar);
  void readTimeStr();
  bool success = false;

public:
  int hours, minutes;
  String usTimeStr = "";
  Gps(SoftwareSerial* ss, LiquidCrystal_I2C* lcd);
  bool waitForTime(int maxTime);
};