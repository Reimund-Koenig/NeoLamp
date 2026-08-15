
#include "lamphelper.h"
#include <Arduino.h>

LampHelper::LampHelper() {

};

bool LampHelper::is_sleeping(unsigned long sleepUntilTime) {
    if(millis() < sleepUntilTime) { return true; }
    return false;
}

void LampHelper::set_none_sleeping_delay(unsigned long wait,
                                         unsigned long *time) {
    *time = millis() + wait;
}