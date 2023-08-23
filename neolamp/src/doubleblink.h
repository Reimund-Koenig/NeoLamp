#ifndef DOUBLEBLINK
#define DOUBLEBLINK
#include "constants/blink_modes.h"
#include "constants/html_inputs.h"
#include "constants/pins.h"
#include "lampfilesystem.h"
#include "lamphelper.h"
#include <Arduino.h>

#define D_BLINK_OFF 0
#define D_BLINK_SWITCH_YELLOW_LED_ON 2
#define D_BLINK_SWITCH_BLUE_LED_ON 3
#define D_BLINK_DO_NOTHING 4

class Doubleblink {
  public:
    Doubleblink(LampFileSystem *lfs);
    void init_blink();
    void loop();
    void start(String mode);
    void stop();
    void set_interval(uint16_t interval);

  private:
    int get_state();
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
    LampFileSystem *lfs;
};
#endif // DOUBLEBLINK
