#include <RtcDS1302.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include <SoftwareSerial.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include "pitches.h"
#include "ButtonController.h"
#include "AlarmSetter.cpp"
#include "Gps.h"


#define TEMPERATURE_PIN 2
OneWire oneWire(TEMPERATURE_PIN);
DallasTemperature sensors(&oneWire);

#define BUZZER 3

ThreeWire myWire(12, 11, 13); // DAT, CLK, RST
RtcDS1302<ThreeWire> rtc(myWire);

LiquidCrystal_I2C lcd(0x27,  16, 2);

#define CYCLE_TIME 200

ButtonController buttonController(8, 9, 10, CYCLE_TIME);
AlarmSetter alarmSetter(&lcd);


SoftwareSerial gpsSs(5, 4);
Gps gps(&gpsSs, &lcd);

void setup() {
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);

  rtc.Begin();
  validateRtc();
  
  lcd.init();
  lcd.backlight();

  if (gps.waitForTime(25 * 1000)) {
    RtcDateTime dateTime = RtcDateTime(__DATE__, gps.usTimeStr.c_str());
    rtc.SetDateTime(dateTime);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("GPS did not work");
    lcd.clear();
    delay(500);
  }

  alarmSetter.loadAlarmFromEEPROM();
  buttonController.begin();


  sensors.begin();
}


void loop() {
  RtcDateTime now = rtc.GetDateTime();
  buttonController.processInput();

  if (buttonController.stateChanged())
    alarmSetter.changeState();
  
  if (alarmSetter.state == NORMAL) {
    showTime(now);
    showTemp();
  } else {
    if (buttonController.triggerInc()) alarmSetter.inc();
    else if (buttonController.triggerDec()) alarmSetter.dec();
    alarmSetter.display();
  }

  if (isAlarmTime(now) && alarmSetter.state == NORMAL
      && !buttonController.alarmJustCancelled) {
    playAlarm();
  } else {
    if (!isAlarmTime(now)) buttonController.alarmJustCancelled = false;
    delay(CYCLE_TIME);
  }
}




int melody[] = {
    NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
int noteDurations[] = {
    4, 8, 8, 4, 4, 4, 4, 4};
void playAlarm()
{
  for (int thisNote = 0; thisNote < 8; thisNote++)
  {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZER, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    buttonController.processInput();
    buttonController.listenForStop(pauseBetweenNotes);
    noTone(BUZZER);
    if (buttonController.alarmJustCancelled) return;
  }
}




bool isAlarmTime(RtcDateTime now)
{
    return (
      now.Hour() == alarmSetter.alarmHour 
      && now.Minute() == alarmSetter.alarmMinute 
    );
}

void validateRtc()
{
    RtcDateTime compiledDateTime = RtcDateTime(__DATE__, __TIME__);
    if (!rtc.IsDateTimeValid())
    {
        Serial.println("RTC lost confidence in the DateTime!");
        rtc.SetDateTime(compiledDateTime);
    }

    if (rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        rtc.SetIsWriteProtected(false);
    }

    if (!rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        rtc.SetIsRunning(true);
    }
}

void showTime(RtcDateTime now)
{
    lcd.setCursor(4,0);
    int hour = now.Hour();
    int min = now.Minute();
    int sec = now.Second();

    if (hour < 10) lcd.print('0');
    lcd.print(hour);
    lcd.print(":");
    if (min < 10) lcd.print('0');
    lcd.print(min);
    lcd.print(":");
    if (sec < 10) lcd.print('0');
    lcd.print(sec);

}
void showTemp() {
  sensors.requestTemperatures();  
  float tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C) {
    lcd.setCursor(5, 1);
    lcd.print(tempC);
    lcd.print((char)0xdf);
    lcd.print("C");
  }
}