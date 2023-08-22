#ifndef DOUBLEBLINK
#define DOUBLEBLINK
#include "clocktime.h"
#include "lamphelper.h"
#include <Arduino.h>

#define D_BLINK_OFF 0
#define D_BLINK_SWITCH_YELLOW_LED_ON 2
#define D_BLINK_SWITCH_BLUE_LED_ON 3
#define D_BLINK_DO_NOTHING 4

class Doubleblink {
  public:
    Doubleblink();
    void start(String mode);
    void stop();
    int get_state();
    void set_interval(uint16_t interval);

  private:
    String mode = "";
    bool isStableColorReturned = false;
    bool doNotBlink = true;
    bool setDoNotBlink = false;
    bool switchHelper = false;
    LampHelper helper;
    uint16_t blink_interval = 500;
    int get_state_blink();
    int get_state_blue_blink();
    int get_state_yellow_blink();
};
#endif // DOUBLEBLINK
