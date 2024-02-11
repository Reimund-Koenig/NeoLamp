
#include "lamptimer.h"
#include <Arduino.h>

LampTimer::LampTimer(LampFileSystem *lfs, Adafruit_NeoPixel *strip) {
    isTimerRunning = 0;
    timerCount = 0;
    timerSeconds = 0;
    timerSteps = 0;
    timerSubSteps = 0;
    timerLastPixelBrightness = 0;
    clock_sleep = 0;
    this->strip = strip;
    // Timer Time
    String tmp_time = lfs->read_file(TIMER_TIME_FS);
    if(tmp_time == "" || tmp_time == NULL) {
        tmp_time = "00:05:00";
        lfs->write_file(TIMER_TIME_FS, tmp_time.c_str());
    }
    timerColor = strip->Color(255, 0, 0);
    setTimerSeconds(tmp_time);
}

void LampTimer::toggleIsRunning() {
    isTimerRunning = !isTimerRunning;
    timerCount = 0;
    timerLastPixelBrightness = 255;
    timerPixel = 15;
}

bool LampTimer::getIsTimerRunning() { return isTimerRunning; }

void LampTimer::timer_loop() {
    if(helper.is_sleeping(clock_sleep)) { return; }
    timerCount++;
    timerSubCount++;
    if(timerCount >= timerSeconds) {
        timerFinish();
        toggleIsRunning();
    } else {
        run();
    }
    helper.set_none_sleeping_delay(1000, &clock_sleep);
}

void LampTimer::run() {
    if(timerLastPixelBrightness > 0) {
        timerLastPixelBrightness -= timerSubSteps;
    } else {
        timerLastPixelBrightness = 0;
    }
    if(timerSubCount >= timerSteps && timerPixel > 0) {
        timerPixel -= 1;
        timerLastPixelBrightness = 255;
        timerSubCount -= timerSteps;
    }
    for(int i = 0; i < 16; i++) {
        if(i < timerPixel) {
            strip->setPixelColor(i, strip->Color(0, 255, 0));
        } else if(i == timerPixel) {
            uint8_t b = (uint8_t)timerLastPixelBrightness;
            strip->setPixelColor(i, strip->Color(255 - b, b, 0));
        } else {
            strip->setPixelColor(i, strip->Color(255, 0, 0));
        }
    }
    strip->show();
}

void LampTimer::timerFinish() {
    for(int i = 0; i < 20; i++) {
        strip->setBrightness(255);
        strip->fill(timerColor);
        strip->show();
        delay(200);
        strip->setBrightness(0);
        strip->fill(timerColor);
        strip->show();
        delay(200);
    }
}
void LampTimer::setTimerSeconds(String t) {
    timerTime.setTime(t.c_str());
    timerSeconds = (timerTime.getHour() * 60 * 60) +
                   (timerTime.getMinutes() * 60) + timerTime.getSeconds();
    if(timerSeconds == 0) {
        timerSteps = 0;
        timerSubSteps = 0;
    } else {
        timerSteps = timerSeconds / 16.0;
        timerSubSteps = 255.0 / timerSteps;
    }
}
