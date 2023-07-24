#ifndef DOUBLEBLINK
#define DOUBLEBLINK
#include "clocktime.h"
#include "lamphelper.h"
#include <Arduino.h>

#define D_BLINK_OFF 0
#define D_BLINK_SWITCH_LED_1_ON 2
#define D_BLINK_SWITCH_LED_2_ON 3
#define D_BLINK_DO_NOTHING 4

class Doubleblink {
  public:
    Doubleblink();
    void start();
    void stop();
    int get_state();

  private:
    bool doNotBlink = true;
    bool setDoNotBlink = false;
    bool switchHelper = false;
    LampHelper helper;
};
#endif // DOUBLEBLINK