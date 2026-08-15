#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
/*
 ToDo Remove for HTTP Update:
#include <AsyncElegantOTA.h> // https://github.com/me-no-dev/ESPAsyncWebServer
*/

#include "src/constants/html_inputs.h"
#include "src/constants/modes.h"
#include "src/constants/modes_array.h"
#include "src/constants/settings.h"
#include "src/lampfilesystem.h"
#include "src/lamphelper.h"


/************************************************************************************************************
/*
/* Finns Nachtlampe (including a Poti for brightness and a Switch to switch off)
/*
*************/

// Needed input values
#define BUTTON_PIN 12
#define STEPS 50
void handleButton();
void handlePotiBrightnessInput();

/************************************************************************************************************
/*
/* Header
/*
*************/
void stateMachine();
void animationStateMachine(String substate);

void updateState(int new_state);
void updateWakeupState(String new_state);
void updateDaytimeState(String new_state);
void updateSleepState(String new_state);

void run_colorPick_mode();
void run_wakeupTime_mode();
void run_sleepingTime_mode();

void run_mixed();
void run_circle();
void run_pulse();
void run_rainbow();
void run_lamp_off();

void createRandomColor();
uint32_t getRgbColor(uint8_t r, uint8_t g, uint8_t b);
void setLampBrightness(uint8_t brightness);
void setLampColorAndBrightness(uint32_t color, uint8_t brightness);
void setLampError();
bool colorCircle(unsigned long wait);
bool colorPulse(unsigned long wait);
bool rainbowCircle(int wait);

void initModes();
void initBrightness();
void initColors();

void updateColorBrightness(uint8_t inputBrightness);
void updateColorPicker(String state, const char *file);

void updateWakeupBrightness(String val);
void updateDaytimeBrightness(String val);
void updateSleepBrightness(String val);
