#ifndef LAMPTIMER
#define LAMPTIMER
#include "clocktime.h"
#include "constants/html_inputs.h"
#include "lampfilesystem.h"
#include "lamphelper.h"
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

class LampTimer {
  public:
    LampTimer(LampFileSystem *lfs, Adafruit_NeoPixel *strip);
    void timer_loop();
    void toggleIsRunning();
    void setTimerSeconds(String t);
    bool getIsTimerRunning();

  private:
    Clocktime timerTime;
    bool isTimerRunning;
    uint32_t timerCount;
    uint32_t timerSeconds;
    uint32_t timerSteps;
    uint32_t timerColor;
    float timerSubSteps;
    float timerLastPixelBrightnessFloat;
    uint8_t timerLastPixelBrightness;
    uint8_t numPixelOn;
    unsigned long clock_sleep;
    LampHelper helper;
    Adafruit_NeoPixel *strip;
    void timerFinish();
    void run();
};
#endif // LAMPTIMER
