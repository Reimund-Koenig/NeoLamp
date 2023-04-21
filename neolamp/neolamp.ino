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
// #include <WiFiUdp.h>
#include "src/clocktime.h"
#include "src/constants/index.html.h"
#include "src/constants/modes.h"
#include "src/constants/secrets.h"
#include "src/constants/timezones.h"
#include "time.h"

#define STATE_SLEEPING_TIME 0
#define STATE_WAKEUP_TIME 1
#define STATE_DAY_TIME_1_CHOOSE_PULSE_OR_WIPE 2
#define STATE_DAY_TIME_2_PULSE_CHOOSE_MIXED_OR_SINGLE_COLOR 3
#define STATE_DAY_TIME_2A_PULSE_CHOOSE_MIXED_COLOR 4
#define STATE_DAY_TIME_2B_PULSE_CHOOSE_SINGLE_COLOR 5
#define STATE_DAY_TIME_3_WIPE_CHOOSE_MIXED_OR_SINGLE_COLOR 6
#define STATE_DAY_TIME_3A_WIPE_CHOOSE_MIXED_COLOR 7
#define STATE_DAY_TIME_3B_WIPE_CHOOSE_SINGLE_COLOR 8
#define STATE_LEARNING 9

#define NEOPIXEL_PIN 4
#define NEOPIXEL_COUNT 16
#define STEPS 32 // 2^x
#define MULTIPLICATOR 256 / STEPS
#define ULONG_MAX (LONG_MAX * 2UL + 1UL)

AsyncWebServer server(80);
DNSServer dns;

Adafruit_NeoPixel strip(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

struct tm timeinfo;

uint8_t colorBrightness = 64; // Set BRIGHTNESS to about 1/5 (max = 255)
uint8_t lastColorBrightness = colorBrightness;

unsigned long time_from_start = 0;
unsigned long sleep_till_time = 0;

uint8_t state = 0;
uint8_t last_state = 0; // State which was choosen last
bool state_first_run = true;
uint8_t learning_mode_substate = 0;

int buttonState = false;
int lastButtonState = LOW;

int colorWipe_helper_i = 0;
int colorPulse_helper_i = 255;
int colorPulse_helper_j = 12;
int colorPulse_helper_k = 255;
int colorPulse_helper_l = 12;
uint32_t random_color;
uint32_t choose_pulse_wipe_counter = 0;
int createRandomColor_helper;

const char *input_sleep_time = "input_sleep_time";
const char *input_wakeup_time = "input_wakeup_time";
const char *input_animation_time = "input_animation_time";
const char *input_animation = "input_animation";
const char *input_timezone = "input_timezone";
const char *input_brightness = "input_brightness";

Clocktime wakeup_time;
Clocktime bed_time;
Clocktime animation_time;
Clocktime current_time;

String read_file(fs::FS &fs, const char *path) {
    Serial.printf("Reading file: %s\r\n", path);
    File file = fs.open(path, "r");
    if(!file || file.isDirectory()) {
        Serial.println("Empty file/Failed to open file");
        return String();
    }
    Serial.println("- read from file:");
    String fileContent;
    while(file.available()) {
        fileContent += String((char)file.read());
    }
    file.close();
    Serial.println(fileContent);
    return fileContent;
}

void write_file(fs::FS &fs, const char *path, const char *message) {
    Serial.printf("Writing file: %s\r\n", path);
    File file = fs.open(path, "w");
    if(!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)) {
        Serial.println("SUCCESS in writing file");
    } else {
        Serial.println("FAILED to write file");
    }
    file.close();
}
void updateTimeZone() {
    String value = read_file(SPIFFS, "/input_timezone.txt");
    if(value == "" || value == NULL) {
        value = "Berlin";
    };
    for(int i = 0;
        i < sizeof(array_of_timezones) / sizeof(array_of_timezones[0]); i++) {
        if(value == array_of_timezones[i][0]) {
            Serial.print("Set Timezone to: ");
            Serial.println(array_of_timezones[i][1]);
            setenv("TZ", array_of_timezones[i][1], 1);
            tzset();
            updateTime();
            return;
        }
    }
}
String processor(const String &var) {
    if(var == "input_sleep_time") {
        return read_file(SPIFFS, "/input_sleep_time.txt");
    } else if(var == "input_wakeup_time") {
        return read_file(SPIFFS, "/input_wakeup_time.txt");
    } else if(var == "input_animation_time") {
        return read_file(SPIFFS, "/input_animation_time.txt");
    } else if(var == "input_animation") {
        String tmp = "";
        String value = read_file(SPIFFS, "/input_animation.txt");
        if(value == "" || value == NULL) {
            value = "mix";
        };
        for(int i = 0; i < sizeof(array_of_modes) / sizeof(array_of_modes[0]);
            i++) {
            tmp += "<option value = '";
            tmp += array_of_modes[i][1];
            if(value == array_of_modes[i][1]) {
                tmp += "' selected>";
            } else {
                tmp += "'>";
            }
            tmp += array_of_modes[i][0];
            tmp += "</ option>";
        }
        return tmp;
    } else if(var == "input_timezone") {
        String tmp = "";
        String value = read_file(SPIFFS, "/input_timezone.txt");
        if(value == "" || value == NULL) {
            value = "Europe_Berlin";
        };
        for(int i = 0;
            i < sizeof(array_of_timezones) / sizeof(array_of_timezones[0]);
            i++) {
            tmp += "<option value = '";
            tmp += array_of_timezones[i][0];
            if(value == array_of_timezones[i][0]) {
                tmp += "' selected>";
            } else {
                tmp += "'>";
            }
            tmp += array_of_timezones[i][0];
            tmp += "</ option>";
        }
        return tmp;
    } else if(var == "input_brightness") {
        return read_file(SPIFFS, "/input_brightness.txt");
    } else if(var == "input_time_on_load") {
        updateTime();
        return current_time.getTimeString();
    }
    return "";
}

/************************************************************************************************************
/*
/* Header
/*
*************/
void test();
void stateMachine();

void run_wakeupTime_mode();
void run_sleepingTime_mode();
void run_learning_mode();

void run_dayTime_mode_1_choosePulseOrWipe();
void run_dayTime_mode_2_Pulse_choose_mixed_or_single_color();
void run_dayTime_mode_2a_Pulse_mixed_color();
void run_dayTime_mode_2b_Pulse_single_color();
void run_dayTime_mode_3_Wipe_choose_mixed_or_single_color();
void run_dayTime_mode_3a_Wipe_mixed_color();
void run_dayTime_mode_3b_Wipe_single_color();

void handleDayTime();

void setNoneSleepingDelay(unsigned long sleepTime);
bool isNoneSleepingDelayOver();
void changeState(int new_state);
void getBrightnessFromPoti();
void createRandomColor();
void printTime();
void printAnalog(int a0);

bool colorWipe(uint32_t color, unsigned long wait);
bool colorPulse(uint32_t color, unsigned long wait);

void initTime();
// void handleRoot();              // function prototypes for HTTP handlers
// void handleNotFound();

void async_wlan_setup();

// void handle_server_root();
// void handle_server_input1();
// void handle_server_input2();
// void handle_server_input3();
void handle_server_notFound(AsyncWebServerRequest *request);
/************************************************************************************************************
/*
/* Arduino Functions
/*
*************/
void setup() {
    // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
    // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif
    Serial.begin(115200);
    if(!SPIFFS.begin()) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    WiFi.mode(WIFI_STA);
    WiFi.hostname("kinderlampe");
    async_wlan_setup();

    // this resets all the neopixels to an off state
    strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();  // Turn OFF all pixels ASAP
    initTime();
    while(WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    delay(10000);
    if(MDNS.begin(
           "kinderlampe")) { // Start the mDNS responder for kinderlampe.local
        Serial.println("mDNS responder started");
    } else {
        Serial.println("Error setting up MDNS responder!");
    }

    MDNS.addService("http", "tcp", 80);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html, processor);
    });

    server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
        String tmp;
        if(request->hasParam(input_sleep_time)) {
            tmp = request->getParam(input_sleep_time)->value();
            write_file(SPIFFS, "/input_sleep_time.txt", tmp.c_str());
        }
        if(request->hasParam(input_wakeup_time)) {
            tmp = request->getParam(input_wakeup_time)->value();
            write_file(SPIFFS, "/input_wakeup_time.txt", tmp.c_str());
        }
        if(request->hasParam(input_animation_time)) {
            tmp = request->getParam(input_animation_time)->value();
            write_file(SPIFFS, "/input_animation_time.txt", tmp.c_str());
        }
        if(request->hasParam(input_animation)) {
            tmp = request->getParam(input_animation)->value();
            write_file(SPIFFS, "/input_animation.txt", tmp.c_str());
        }
        if(request->hasParam(input_timezone)) {
            tmp = request->getParam(input_timezone)->value();
            write_file(SPIFFS, "/input_timezone.txt", tmp.c_str());
            updateTimeZone();
        }
        if(request->hasParam(input_brightness)) {
            tmp = request->getParam(input_brightness)->value();
            write_file(SPIFFS, "/input_brightness.txt", tmp.c_str());
        }
        request->send(200, "text/text", "ok");
    });
    server.onNotFound(handle_server_notFound);
    server.begin(); // Actually start the server
    Serial.println("HTTP server started");
    // Get Current Hostname
    Serial.print("Default hostname: ");
    Serial.println(WiFi.hostname().c_str());

    Serial.print("Connected to ");
    Serial.println(WiFi.SSID()); // Tell us what network we're connected to
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());
}

void loop() {
    MDNS.update();
    if(!isNoneSleepingDelayOver()) {
        return;
    }
    String str_sleep_time = read_file(SPIFFS, "/input_sleep_time.txt");
    String str_wakeup_time = read_file(SPIFFS, "/input_wakeup_time.txt");
    String str_animation_time = read_file(SPIFFS, "/input_animation_time.txt");
    String input_animation = read_file(SPIFFS, "/input_animation.txt");
    String input_timezone = read_file(SPIFFS, "/input_timezone.txt");
    String input_brightness = read_file(SPIFFS, "/input_brightness.txt");
    Serial.print("str_sleep_time: ");
    Serial.print(str_sleep_time);
    Serial.print(", str_wakeup_time: ");
    Serial.print(str_wakeup_time);
    Serial.print(", str_animation_time: ");
    Serial.print(str_animation_time);
    Serial.print(", input_animation: ");
    Serial.print(input_animation);
    Serial.print(", input_timezone: ");
    Serial.print(input_timezone);
    Serial.print(", input_brightness: ");
    Serial.print(input_brightness);
    Serial.println("....");

    // handleDayTime();
    // stateMachine();
    updateTime();
    int h = timeinfo.tm_hour;
    int m = timeinfo.tm_min;
    Serial.print(h);
    Serial.print(":");
    Serial.println(m);
    setNoneSleepingDelay(5000);
}

/************************************************************************************************************
/*
/* Test
/*
*************/

void handle_server_notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

/*
void handle_server_root() {
    server.send(200, "text/html", index_html);
  // Timezone - default: Berlin
  // Go to bed time - default: 19:00 Uhr
  // Wakeup time - default: 8:00 Uhr
  // Daymode: default: Farbkeise und Pulsieren
  // modes: off, green, Farbkreise, Pulsieren, Farbkeise und Pulsieren
}



void handle_server_input1 () {
  Serial.println("Input 1");
   if(!server.hasArg("input1") || server.arg("input1") == NULL) {
      Serial.println(server.arg("ERR: No input1 or NULL"));
      server.send(400, "text/plain", "400: Invalid Request");         // The
request is invalid, so send HTTP status 400 return;
  }
  Serial.println(server.arg("input1"));
  server.sendHeader("Location","/");        // Add a header to respond with a
new location for the browser to go to the home page again server.send(303); //
Send it back to the browser with an HTTP status 303 (See Other) to redirect
}
void handle_server_input2 () {
  Serial.println("Input 2");
  server.sendHeader("Location","/");        // Add a header to respond with a
new location for the browser to go to the home page again server.send(303); //
Send it back to the browser with an HTTP status 303 (See Other) to redirect
}
void handle_server_input3 () {
  Serial.println("Input 3");
  server.sendHeader("Location","/");        // Add a header to respond with a
new location for the browser to go to the home page again server.send(303); //
Send it back to the browser with an HTTP status 303 (See Other) to redirect
}
*/

// ToDo: rainbowFade(3, 3);
void test() {
    for(int i = 0; i < strip.numPixels(); i++) {
        if(i > 10) {
            strip.setPixelColor(
                i, strip.Color(0, 128, 255, colorBrightness)); // Heaven_Blue
        } else {
            strip.setPixelColor(
                i, strip.Color(255, 0, 128, colorBrightness)); // Pink
        }
    }
    if(colorPulse(0, 5)) {
        // Back to start
    }
}

/************************************************************************************************************
/*
/* Modes
/*
*************/
void run_dayTime_mode_1_choosePulseOrWipe() {
    if(choose_pulse_wipe_counter < 100) {
        changeState(STATE_DAY_TIME_3_WIPE_CHOOSE_MIXED_OR_SINGLE_COLOR);
    } else {
        changeState(STATE_DAY_TIME_2_PULSE_CHOOSE_MIXED_OR_SINGLE_COLOR);
    }
    if(choose_pulse_wipe_counter > 110) {
        choose_pulse_wipe_counter = 0;
    }
    choose_pulse_wipe_counter++;
}

void run_dayTime_mode_2_Pulse_choose_mixed_or_single_color() {
    if(random(0, 2) == 0) {
        changeState(STATE_DAY_TIME_2A_PULSE_CHOOSE_MIXED_COLOR);
    } else {
        changeState(STATE_DAY_TIME_2B_PULSE_CHOOSE_SINGLE_COLOR);
    }
}

void run_dayTime_mode_2a_Pulse_mixed_color() {
    // ToDo
    changeState(STATE_DAY_TIME_1_CHOOSE_PULSE_OR_WIPE);
}

void run_dayTime_mode_2b_Pulse_single_color() {
    if(state_first_run) {
        createRandomColor();
        state_first_run = false;
    }
    if(colorPulse(random_color, 17)) {
        changeState(STATE_DAY_TIME_1_CHOOSE_PULSE_OR_WIPE);
    }
}

void run_dayTime_mode_3_Wipe_choose_mixed_or_single_color() {
    getBrightnessFromPoti();
    strip.setBrightness(colorBrightness);
    strip.show();
    if(random(0, 2) == 0) {
        changeState(STATE_DAY_TIME_3A_WIPE_CHOOSE_MIXED_COLOR);
    } else {
        changeState(STATE_DAY_TIME_3B_WIPE_CHOOSE_SINGLE_COLOR);
    }
}

void run_dayTime_mode_3a_Wipe_mixed_color() {
    // ToDo
    changeState(STATE_DAY_TIME_1_CHOOSE_PULSE_OR_WIPE);
}

void run_dayTime_mode_3b_Wipe_single_color() {
    if(state_first_run) {
        createRandomColor();
        state_first_run = false;
    }
    if(colorWipe(random_color, 100)) {
        changeState(STATE_DAY_TIME_1_CHOOSE_PULSE_OR_WIPE);
    }
}

void run_wakeupTime_mode() {
    if(state_first_run) {
        strip.fill(strip.Color(0, 75, 0, colorBrightness));
        strip.setPixelColor(3, strip.Color(0, 255, 0, colorBrightness));
        strip.setPixelColor(4, strip.Color(0, 255, 0, colorBrightness));
        strip.setPixelColor(5, strip.Color(0, 255, 0, colorBrightness));
        strip.setPixelColor(6, strip.Color(0, 255, 0, colorBrightness));
        strip.setPixelColor(7, strip.Color(0, 255, 0, colorBrightness));
        strip.setPixelColor(8, strip.Color(0, 255, 0, colorBrightness));
        strip.show();
        state_first_run = false;
    }
}

void run_sleepingTime_mode() {
    if(state_first_run) {
        strip.fill(strip.Color(255, 75, 0, colorBrightness));
        strip.setPixelColor(3, strip.Color(255, 9, 0, colorBrightness));
        strip.setPixelColor(4, strip.Color(255, 18, 0, colorBrightness));
        strip.setPixelColor(5, strip.Color(255, 37, 0, colorBrightness));
        strip.setPixelColor(6, strip.Color(255, 0, 0, colorBrightness));
        strip.setPixelColor(7, strip.Color(255, 0, 0, colorBrightness));
        strip.setPixelColor(8, strip.Color(255, 0, 0, colorBrightness));
        strip.show();
        state_first_run = false;
    }
}

void run_learning_mode() {
    if(!isNoneSleepingDelayOver()) {
        return;
    }
    if(learning_mode_substate == 0) {
        state_first_run = true;
        run_wakeupTime_mode();
        learning_mode_substate = 1;
    } else if(learning_mode_substate == 1) {
        state_first_run = true;
        run_sleepingTime_mode();
        learning_mode_substate = 0;
    }
    setNoneSleepingDelay(3000);
}

/************************************************************************************************************
/*
/* Main Functions
/*
*************/

void stateMachine() {
    if(state == STATE_SLEEPING_TIME) {
        run_sleepingTime_mode();
    } else if(state == STATE_WAKEUP_TIME) {
        run_wakeupTime_mode();
    } else if(state == STATE_DAY_TIME_1_CHOOSE_PULSE_OR_WIPE) {
        run_dayTime_mode_1_choosePulseOrWipe();
    } else if(state == STATE_DAY_TIME_2_PULSE_CHOOSE_MIXED_OR_SINGLE_COLOR) {
        run_dayTime_mode_2_Pulse_choose_mixed_or_single_color();
    } else if(state == STATE_DAY_TIME_2A_PULSE_CHOOSE_MIXED_COLOR) {
        run_dayTime_mode_2a_Pulse_mixed_color();
    } else if(state == STATE_DAY_TIME_2B_PULSE_CHOOSE_SINGLE_COLOR) {
        run_dayTime_mode_2b_Pulse_single_color();
    } else if(state == STATE_DAY_TIME_3_WIPE_CHOOSE_MIXED_OR_SINGLE_COLOR) {
        run_dayTime_mode_3_Wipe_choose_mixed_or_single_color();
    } else if(state == STATE_DAY_TIME_3A_WIPE_CHOOSE_MIXED_COLOR) {
        run_dayTime_mode_3a_Wipe_mixed_color();
    } else if(state == STATE_DAY_TIME_3B_WIPE_CHOOSE_SINGLE_COLOR) {
        run_dayTime_mode_3b_Wipe_single_color();
    } else if(state == STATE_LEARNING) {
        run_learning_mode();
    } else {
        strip.fill(strip.Color(255, 0, 0, 255));
        strip.show();
    }
}

/************************************************************************************************************
/*
/* Inputs
/*
*************/

void handleDayTime() {
    if(!isNoneSleepingDelayOver()) {
        return;
    }
    updateTime();
    int h = timeinfo.tm_hour;
    int m = timeinfo.tm_min;
    Serial.print(h);
    Serial.print(":");
    Serial.println(m);
    if(h >= 18) { // Schlafen 19:00 - 24:00 Uhr
        changeState(STATE_SLEEPING_TIME);
    } else if(h >= 10) {
        if(state == STATE_DAY_TIME_1_CHOOSE_PULSE_OR_WIPE ||
           state == STATE_DAY_TIME_2_PULSE_CHOOSE_MIXED_OR_SINGLE_COLOR ||
           state == STATE_DAY_TIME_2A_PULSE_CHOOSE_MIXED_COLOR ||
           state == STATE_DAY_TIME_2B_PULSE_CHOOSE_SINGLE_COLOR ||
           state == STATE_DAY_TIME_3_WIPE_CHOOSE_MIXED_OR_SINGLE_COLOR ||
           state == STATE_DAY_TIME_3A_WIPE_CHOOSE_MIXED_COLOR ||
           state == STATE_DAY_TIME_3B_WIPE_CHOOSE_SINGLE_COLOR) {
            return; // only start mode once
        }
        changeState(STATE_DAY_TIME_1_CHOOSE_PULSE_OR_WIPE);
    } else if(h > 6 ||
              (h == 6 &&
               m >= 45)) { // Gleich Zeit zum Aufstehen 06:45 - 10:00 Uhr
        changeState(STATE_WAKEUP_TIME);
    } else if(h < 6 || (h == 6 && m < 45)) { // Schlafen 0:00 - 6:45 Uhr
        changeState(STATE_SLEEPING_TIME);
    }
    setNoneSleepingDelay(1000);
}

/************************************************************************************************************
/*
/* HELPER
/*
*************/

void async_wlan_setup() {
    Serial.begin(115200);
    // Local intialization. Once its business is done, there is no need to keep
    // it around
    AsyncWiFiManager wifiManager(&server, &dns);
    // reset saved settings >> USED TO TEST
    //  wifiManager.resetSettings();
    wifiManager.autoConnect("Kinder Lampe");
}

void updateTime() {
    if(!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time 1");
        return;
    }
    current_time.setTime(timeinfo.tm_hour, timeinfo.tm_min);
}

void initTime() {
    configTime(0, 0, "pool.ntp.org");
    updateTime();
    // Now we can set the real timezone
    // https://randomnerdtutorials.com/esp32-ntp-timezones-daylight-saving/
    // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
    updateTimeZone();
}

void setNoneSleepingDelay(unsigned long sleepTime) {
    sleep_till_time = time_from_start + sleepTime;
}

bool isNoneSleepingDelayOver() {
    time_from_start = millis();
    if(time_from_start < sleep_till_time) {
        return false;
    }
    return true;
}

void changeState(int new_state) {
    if(last_state != STATE_LEARNING) {
        last_state = state;
    }
    state = new_state;
    state_first_run = true;
}
void getBrightnessFromPoti() {
    // Values from 0-255
    int a0 = analogRead(A0);
    if(a0 < STEPS) {
        colorBrightness = 1;
    } else {
        colorBrightness = (int(a0 / STEPS) * MULTIPLICATOR) - 1;
    }
    // printAnalog(a0);
}

void createRandomColor() {
    int r1 = random(0, 4);
    int r2 = random(0, 2);
    while(r2 == createRandomColor_helper) {
        r2 = random(0, 2);
    }
    createRandomColor_helper = r2;
    if(r1 == 0) {
        if(createRandomColor_helper == 0) {
            random_color = strip.Color(random(150, 256), random(0, 150), 0);
        } else {
            random_color = strip.Color(random(150, 256), 0, random(0, 150));
        }
    } else if(r1 == 1) {
        if(createRandomColor_helper == 0) {
            random_color = strip.Color(0, random(150, 256), random(0, 150));
        } else {
            random_color = strip.Color(random(0, 150), random(150, 256), 0);
        }
    } else {
        if(createRandomColor_helper == 0) {
            random_color = strip.Color(0, random(0, 150), random(30, 256));
        } else {
            random_color = strip.Color(random(0, 150), 0, random(30, 256));
        }
    }
}

void printAnalog(int a0) {
    Serial.print("Analog Read: ");
    Serial.print(a0);
    Serial.print("colorBrightness: ");
    Serial.println(colorBrightness);
}

/************************************************************************************************************
/*
/* Adafruit NeoPixel Standard Functions
/*
*************/

bool colorWipe(uint32_t color, int wait) {
    if(!isNoneSleepingDelayOver()) {
        return false;
    }
    if(colorWipe_helper_i >= strip.numPixels()) {
        colorWipe_helper_i = 0;
        return true;
    }
    strip.setPixelColor(colorWipe_helper_i,
                        color); //  Set pixel's color (in RAM)
    strip.show();               //  Update strip to match
    colorWipe_helper_i++;
    setNoneSleepingDelay(wait);
    return false;
}

bool colorPulse(uint32_t color, unsigned long wait) {
    if(!isNoneSleepingDelayOver()) {
        return false;
    }
    if(color > 0) {
        strip.fill(color);
        strip.show();
    }
    if(colorPulse_helper_i > 12) {
        colorPulse_helper_i--;
        strip.setBrightness(colorPulse_helper_i);
    } else if(colorPulse_helper_j < 255) {
        colorPulse_helper_j++;
        strip.setBrightness(colorPulse_helper_j);
    } else if(colorPulse_helper_k > 12) {
        colorPulse_helper_k--;
        strip.setBrightness(colorPulse_helper_k);
        if(colorPulse_helper_k == 13) {
            return true;
        }
    } else {
        colorPulse_helper_l++;
        strip.setBrightness(colorPulse_helper_l);
        if(colorPulse_helper_l >= 255) {
            colorPulse_helper_i = 255;
            colorPulse_helper_j = 12;
            colorPulse_helper_k = 255;
            colorPulse_helper_l = 12;
        }
    }
    strip.show();
    setNoneSleepingDelay(wait);
    return false;
}

/*

void rainbowFade(int wait, int rainbowLoops) {
  int fadeVal=0, fadeMax=100;

  // Hue of first pixel runs 'rainbowLoops' complete loops through the color
  // wheel. Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to rainbowLoops*65536, using steps of 256 so we
  // advance around the wheel at a decent clip.
  for(uint32_t firstPixelHue = 0; firstPixelHue < rainbowLoops*65536;
    firstPixelHue += 256) {

    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...

      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      uint32_t pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());

      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the three-argument variant, though the
      // second value (saturation) is a constant 255.
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue, 255,
        255 * fadeVal / fadeMax)));
    }

    strip.show();
    delay(wait);

    if(firstPixelHue < 65536) {                              // First loop,
      if(fadeVal < fadeMax) fadeVal++;                       // fade in
    } else if(firstPixelHue >= ((rainbowLoops-1) * 65536)) { // Last loop,
      if(fadeVal > 0) fadeVal--;                             // fade out
    } else {
      fadeVal = fadeMax; // Interim loop, make sure fade is at max
    }
  }
  delay(500); // Pause 1/2 second
}
*/
