#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
/*
 ToDo Remove for HTTP Update:
#include <AsyncElegantOTA.h> // https://github.com/me-no-dev/ESPAsyncWebServer
*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h> //https://github.com/tzapu/WiFiManager

#include "src/clocktime.h"
#include "src/constants/blink_mode_array.h"
#include "src/constants/html_inputs.h"
#include "src/constants/modes.h"
#include "src/constants/modes_array.h"
#include "src/constants/secrets.h"
#include "src/constants/settings.h"
#include "src/constants/timezones.h"
#include "src/doubleblink.h"
#include "src/html/index.html.h"
#include "src/html/settings.html.h"
#include "src/html/timer.html.h"
#include "src/lampfilesystem.h"
#include "src/lamphelper.h"
#include "src/lamptimer.h"
#include "src/lampupdater.h"
#include "time.h"

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

void updateStateAndTime();

void createRandomColor();
uint32_t getRgbColor(uint8_t r, uint8_t g, uint8_t b);
void setLampBrightness(uint8_t brightness);
void setLampColorAndBrightness(uint32_t color, uint8_t brightness);
void setLampError();
bool colorCircle(unsigned long wait);
bool colorPulse(unsigned long wait);
bool rainbowCircle(int wait);

void initTime();
void initModes();
void initBrightness();
void initColors();
void initUserTimes();

void updateColorBrightness(uint8_t inputBrightness);
void updateColorPicker(String state, const char *file);

void updateWakeupBrightness(String val);
void updateDaytimeBrightness(String val);
void updateSleepBrightness(String val);

void updateTimeZone();
void async_wlan_setup();

void handle_server_root(AsyncWebServerRequest *request);
void handle_server_get(AsyncWebServerRequest *request);
void handle_server_notFound(AsyncWebServerRequest *request);
String processor(const String &var);
String read_file(fs::FS &fs, const char *path);
void write_file(fs::FS &fs, const char *path, const char *message);
void printServerInfo();
