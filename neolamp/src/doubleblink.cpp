#include "doubleblink.h"

unsigned long db_clock_sleep = 0;

Doubleblink::Doubleblink(LampFileSystem *lfs) {
    this->lfs = lfs;
    String value = lfs->read_file(SETTING_SPECIAL_LAMP_WITH_DOUBLEBLINK_FS);
    if(value == "" || value == NULL) {
        is_led_connected = false;
        return;
    }
    is_led_connected = true;
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_YELLOW, LOW);
    value = lfs->read_file(WAKEUP_BLINK_FS);
    if(value == "" || value == NULL) {
        value = D_LED_MODE_OFF;
        lfs->write_file(WAKEUP_BLINK_FS, value.c_str());
    }
    value = lfs->read_file(DAYTIME_BLINK_FS);
    if(value == "" || value == NULL) {
        value = D_LED_MODE_BLINK;
        lfs->write_file(DAYTIME_BLINK_FS, value.c_str());
    }
    value = lfs->read_file(SLEEP_BLINK_FS);
    if(value == "" || value == NULL) {
        value = D_LED_MODE_YELLOW;
        lfs->write_file(SLEEP_BLINK_FS, value.c_str());
    }
    value = lfs->read_file(BLINK_INTERVAL_FS);
    if(value == "" || value == NULL) {
        value = "2500";
        lfs->write_file(BLINK_INTERVAL_FS, value.c_str());
    }
    set_interval((uint16_t)(value.toInt()));
}

void Doubleblink::loop() {
    if(!is_led_connected) return;
    int current_state = get_state();
    if(current_state == D_BLINK_DO_NOTHING) { return; }
    if(current_state == D_BLINK_SWITCH_BLUE_LED_ON) {
        digitalWrite(LED_BLUE, HIGH);
        digitalWrite(LED_YELLOW, LOW);
        return;
    }
    if(current_state == D_BLINK_SWITCH_YELLOW_LED_ON) {
        digitalWrite(LED_BLUE, LOW);
        digitalWrite(LED_YELLOW, HIGH);
        return;
    }
    if(current_state == D_BLINK_OFF) {
        digitalWrite(LED_BLUE, LOW);
        digitalWrite(LED_YELLOW, LOW);
    }
}

bool Doubleblink::isActive() { return is_led_connected; }

void Doubleblink::start(String mode) {
    if(!is_led_connected) return;
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

void Doubleblink::updateBlinkState(uint8_t state) {
    if(!is_led_connected) return;
    if(state == STATE_WAKEUP) {
        updateBlink(lfs->read_file(WAKEUP_BLINK_FS));
    } else if(state == STATE_DAYTIME) {
        updateBlink(lfs->read_file(DAYTIME_BLINK_FS));
    } else if(state == STATE_SLEEPING) {
        updateBlink(lfs->read_file(SLEEP_BLINK_FS));
    } else {
        updateBlink("0");
    }
}

void Doubleblink::updateBlink(String value) {
    if(!is_led_connected) return;
    if(value == D_LED_MODE_OFF) {
        stop();
    } else {
        start(value);
    }
}
