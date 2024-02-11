
#include "lamptimer.h"
#include <Arduino.h>

LampTimer::LampTimer(LampFileSystem *lfs, Adafruit_NeoPixel *strip) {
    isTimerRunning = 0;
    timerCount = 0;
    timerSeconds = 16;
    timerSteps = 0;
    timerSubSteps = 0;
    timerLastPixelBrightness = 0;
    clock_sleep = 0;
    this->strip = strip;
    // Timer Time
    String tmp_time = lfs->read_file(TIMER_TIME_FS);
    if(tmp_time == "" || tmp_time == NULL) {
        tmp_time = "00:00";
        lfs->write_file(TIMER_TIME_FS, tmp_time.c_str());
    }
    timerColor = strip->Color(255, 0, 0, 0);
    setTimerSeconds(tmp_time);
};

void LampTimer::toggleIsRunning() {
    isTimerRunning = !isTimerRunning;
    timerCount = 0;
    timerLastPixelBrightnessFloat = 255;
    timerLastPixelBrightness = 255;
    numPixelOn = 16;
}

bool LampTimer::getIsTimerRunning() { return isTimerRunning; }

void LampTimer::timer_loop() {
    if(helper.is_sleeping(clock_sleep)) { return; }
    timerCount++;
    if(timerCount >= timerSeconds) {
        timerFinish();
        toggleIsRunning();
    } else {
        run();
    }
    helper.set_none_sleeping_delay(1000, &clock_sleep);
}

void LampTimer::run() {
    timerLastPixelBrightnessFloat -= timerSubSteps;
    timerLastPixelBrightness = (uint8_t)timerLastPixelBrightnessFloat;
    if(timerCount % timerSteps == 0 && numPixelOn > 0) { numPixelOn -= 1; }
    for(int i = 0; i < 16; i++) {
        if(i < numPixelOn) {
            strip->setPixelColor(i, strip->Color(255, 0, 0, 255));
        } else if(i == numPixelOn) {
            strip->setPixelColor(
                i, strip->Color(255, 0, 0, timerLastPixelBrightness));
        } else {
            strip->setPixelColor(i, timerColor);
        }
    }
    strip->show();
}

void LampTimer::timerFinish() {
    for(int i = 0; i < 10; i++) {
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
    // Use hours as minutes and minutes as seconds
    timerTime.setTime(t.c_str());
    timerSeconds = (timerTime.getHour() * 60) + timerTime.getMinutes();
    if(timerSeconds == 0) {
        timerSteps = 0;
        timerSubSteps = 0;
    } else {
        timerSteps = timerSeconds / 16;
        timerSubSteps = 255.0 / timerSteps;
    }
}
