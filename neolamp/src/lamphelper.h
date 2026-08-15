#ifndef LAMPHELPER
#define LAMPHELPER
#include "clocktime.h"
#include <Arduino.h>

class LampHelper {
  public:
    LampHelper();
    void set_none_sleeping_delay(unsigned long wait,
                                 unsigned long *sleepUntilTime);
    bool is_sleeping(unsigned long sleepUntilTime);
};
#endif // LAMPHELPER
