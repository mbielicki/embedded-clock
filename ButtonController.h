class ButtonController {
  int setPin;
  int incPin;
  int decPin;
  int cyclesToActivateSetting;
  int cyclesPerSec;

public:
  int setPressed, incPressed, decPressed; //number of ticks a button was pressed in a row
  bool alarmJustCancelled = false;

  ButtonController(int setPin, int incPin, int decPin, int cycleTime);
  void begin();
  void processInput();
  void listenForStop(int delayMillis);
  bool stateChanged();
  bool triggerInc();
  bool triggerDec();
};