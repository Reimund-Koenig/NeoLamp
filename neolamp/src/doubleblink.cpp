
#include "doubleblink.h "
#include <Arduino.h>

unsigned long db_clock_sleep = 0;

Doubleblink::Doubleblink(){};

void Doubleblink::start() { this->doNotBlink = false; }
void Doubleblink::stop() { this->setDoNotBlink = true; }
int Doubleblink::get_state() {
    if(this->doNotBlink) { return D_BLINK_DO_NOTHING; }
    if(isSleeping(db_clock_sleep)) { return D_BLINK_DO_NOTHING; }
    setNoneSleepingDelay(2000, &db_clock_sleep);
    if(this->setDoNotBlink) {
        this->doNotBlink = true;
        this->setDoNotBlink = false;
        return D_BLINK_OFF;
    }
    this->switchHelper = !this->switchHelper;
    if(this->switchHelper) { return D_BLINK_SWITCH_LED_1_ON; }
    return D_BLINK_SWITCH_LED_2_ON;
}