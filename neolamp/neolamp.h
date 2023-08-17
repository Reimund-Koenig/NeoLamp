#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <FS.h>
#include <Hash.h>

#include "src/clocktime.h"
#include "src/constants/html_inputs.h"
#include "src/constants/index.html.h"
#include "src/constants/modes.h"
#include "src/constants/secrets.h"
#include "src/constants/timezones.h"
#include "src/doubleblink.h"
#include "src/lamphelper.h"
#include "time.h"

#define NAME "Finns Nachtlicht"
#define URL "finn"
#define STATE_SLEEPING_TIME 0
#define STATE_WAKEUP_TIME 1
#define STATE_DAYTIME_TIME 2

#define STATE_ANIMATION_MIX String(array_of_modes[0][1])
#define STATE_ANIMATION_PULSE String(array_of_modes[1][1])
#define STATE_ANIMATION_CIRCLE String(array_of_modes[2][1])
#define STATE_ANIMATION_RAINBOW String(array_of_modes[3][1])
#define STATE_ANIMATION_PICK String(array_of_modes[4][1])
#define STATE_ANIMATION_GREEN String(array_of_modes[5][1])
#define STATE_ANIMATION_RED String(array_of_modes[6][1])
#define STATE_ANIMATION_OFF String(array_of_modes[7][1])

#define NEOPIXEL_PIN 4 // Wemos D1 mini: port D2
#define LED_1 0        // Wemos D1 mini: port D3
#define LED_2 2        // Wemos D1 mini: port D3

#define NEOPIXEL_COUNT 16
#define LED1_PIN 6 // ToDo
#define LED2_PIN 5 // ToDo

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

void init_blink();
void blinkStateMachine();
void updateBlink(String value);
void updateBlinkState(int new_state);

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
void update_blink_intervall();

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
void blink();
