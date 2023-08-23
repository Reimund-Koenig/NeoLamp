#include "doubleblink.h"

unsigned long db_clock_sleep = 0;

Doubleblink::Doubleblink(){};

/************************************************************************************************************
/*
/* LED Blink Code
/*
*************/

void loop() {
    int current_state = d_blink.get_state();
    if(current_state == D_BLINK_DO_NOTHING) { return; }
    if(current_state == D_BLINK_SWITCH_BLUE_LED_ON) {
        digitalWrite(LED_1, HIGH);
        digitalWrite(LED_2, LOW);
        return;
    }
    if(current_state == D_BLINK_SWITCH_YELLOW_LED_ON) {
        digitalWrite(LED_1, LOW);
        digitalWrite(LED_2, HIGH);
        return;
    }
    if(current_state == D_BLINK_OFF) {
        digitalWrite(LED_1, LOW);
        digitalWrite(LED_2, LOW);
    }
}

void Doubleblink::start(String mode) {
    this->doNotBlink = false;
    this->setDoNotBlink = false;
    this->isStableColorReturned = false;
    this->mode = mode;
}

void Doubleblink::stop() { this->setDoNotBlink = true; }

void Doubleblink::set_interval(uint16_t interval) { blink_interval = interval; }

int Doubleblink::get_state() {
    if(this->doNotBlink) { return D_BLINK_DO_NOTHING; }
    if(helper.is_sleeping(db_clock_sleep)) { return D_BLINK_DO_NOTHING; }
    helper.set_none_sleeping_delay(blink_interval, &db_clock_sleep);
    if(this->setDoNotBlink) {
        this->doNotBlink = true;
        return D_BLINK_OFF;
    }
    if(mode == D_LED_MODE_BLINK)
        return get_state_blink();
    else if(mode == D_LED_MODE_BLUE) {
        if(isStableColorReturned) { return D_BLINK_DO_NOTHING; }
        isStableColorReturned = true;
        return D_BLINK_SWITCH_BLUE_LED_ON;
    } else if(mode == D_LED_MODE_YELLOW) {
        if(isStableColorReturned) { return D_BLINK_DO_NOTHING; }
        isStableColorReturned = true;
        return D_BLINK_SWITCH_YELLOW_LED_ON;
    } else if(mode == D_LED_MODE_BLUE_BLINK) {
        return get_state_blue_blink();
    } else if(mode == D_LED_MODE_YELLOW_BLINK) {
        return get_state_yellow_blink();
    }
    return D_BLINK_OFF;
}

int Doubleblink::get_state_blue_blink() {
    this->switchHelper = !this->switchHelper;
    if(this->switchHelper) { return D_BLINK_OFF; }
    return D_BLINK_SWITCH_BLUE_LED_ON;
}

int Doubleblink::get_state_yellow_blink() {
    this->switchHelper = !this->switchHelper;
    if(this->switchHelper) { return D_BLINK_SWITCH_YELLOW_LED_ON; }
    return D_BLINK_OFF;
}

int Doubleblink::get_state_blink() {
    this->switchHelper = !this->switchHelper;
    if(this->switchHelper) { return D_BLINK_SWITCH_YELLOW_LED_ON; }
    return D_BLINK_SWITCH_BLUE_LED_ON;
}
