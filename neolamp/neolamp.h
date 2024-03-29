#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h> //https://github.com/tzapu/WiFiManager

#include "src/clocktime.h"
#include "src/constants/blink_mode_array.h"
#include "src/constants/html_inputs.h"
#include "src/constants/index.html.h"
#include "src/constants/modes.h"
#include "src/constants/modes_array.h"
#include "src/constants/secrets.h"
#include "src/constants/settings.h"
#include "src/constants/timezones.h"
#include "src/doubleblink.h"
#include "src/lampfilesystem.h"
#include "src/lamphelper.h"
#include "time.h"

#define NEOPIXEL_COUNT 16

/************************************************************************************************************
/*
/* Header
/*
*************/
void stateMachine();
void animationStateMachine(String substate);

void updateState(int new_state);
void change_wakeup_state(String new_state);
void change_daytime_state(String new_state);
void change_sleep_state(String new_state);

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

bool colorCircle(unsigned long wait);
bool colorPulse(unsigned long wait);
bool rainbowCircle(int wait);

void initTime();

void update_daytime_mode();

void update_color_brightness(uint8_t inputBrightness);
void update_color_picker(String state, const char *file);

void update_wakeup_brightness();
void update_daytime_brightness();
void update_sleep_brightness();

void update_sleep_color();
void update_daytime_color();
void update_wakeup_color();

void updateUserTimes();
void updateTimeZone();
void async_wlan_setup();

void handle_server_root(AsyncWebServerRequest *request);
void handle_server_get(AsyncWebServerRequest *request);
void handle_server_notFound(AsyncWebServerRequest *request);
String processor(const String &var);
String read_file(fs::FS &fs, const char *path);
void write_file(fs::FS &fs, const char *path, const char *message);
void printServerInfo();
