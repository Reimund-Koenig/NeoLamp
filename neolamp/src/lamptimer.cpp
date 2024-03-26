
#include "lamptimer.h"
#include <Arduino.h>

LampTimer::LampTimer(Adafruit_NeoPixel *strip, uint8_t pixel_count,
                     String init_time) {
    isTimerRunning = 0;
    timerCount = 0;
    timerSeconds = 0;
    timerSteps = 0;
    timerSubSteps = 0;
    timerLastPixelBrightness = 0;
    clock_sleep = 0;
    this->strip = strip;
    PIXEL_COUNT = pixel_count;
    timerColor = strip->Color(255, 0, 0);
    setTimerSeconds(init_time);
}

void LampTimer::toggleIsRunning() {
    isTimerRunning = !isTimerRunning;
    resetTimerSteps();
    timerCount = 0;
    timerLastPixelBrightness = 255;
    timerPixel = PIXEL_COUNT - 1;
}

bool LampTimer::getIsTimerRunning() { return isTimerRunning; }

void LampTimer::timer_loop() {
    if(helper.is_sleeping(clock_sleep)) { return; }
    timerCount++;
    timerSubCount++;
    if(timerCount >= timerSeconds) {
        timerFinish();
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
    while(timerSubCount >= timerSteps && timerPixel > 0) {
        timerPixel -= 1;
        timerLastPixelBrightness = 255;
        timerSubCount -= timerSteps;
    }
    for(int i = 0; i < PIXEL_COUNT; i++) {
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
    toggleIsRunning();
}

void LampTimer::resetTimerSteps() {
    if(timerSeconds == 0) {
        timerSteps = 0;
        timerSubSteps = 0;
    } else {
        timerSteps = timerSeconds / PIXEL_COUNT;
        timerSubSteps = 255.0 / timerSteps;
    }
}

void LampTimer::setTimerSeconds(String t) {
    timerTime.setTime(t.c_str());
    timerSeconds = (timerTime.getHour() * 60 * 60) +
                   (timerTime.getMinutes() * 60) + timerTime.getSeconds();
    resetTimerSteps();
}
