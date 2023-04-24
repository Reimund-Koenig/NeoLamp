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
#include "src/lamphelper.h"
#include "time.h"

#define STATE_SLEEPING_TIME 0
#define STATE_WAKEUP_TIME 1
#define STATE_ANIMATION_TIME 2

#define STATE_ANIMATION_CIRCLE_PULSE String(array_of_modes[0][1])
#define STATE_ANIMATION_PULSE String(array_of_modes[1][1])
#define STATE_ANIMATION_CIRCLE String(array_of_modes[2][1])
#define STATE_ANIMATION_RAINBOW String(array_of_modes[3][1])
#define STATE_ANIMATION_GREEN String(array_of_modes[4][1])
#define STATE_ANIMATION_OFF String(array_of_modes[5][1])
#define STATE_LEARNING String(array_of_modes[6][1])

#define NEOPIXEL_PIN 4
#define NEOPIXEL_COUNT 16

AsyncWebServer server(80);
DNSServer dns;

Adafruit_NeoPixel strip(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

struct tm timeinfo;

uint8_t colorBrightness = 64; // Set BRIGHTNESS to about 1/5 (max = 255)

unsigned long clock_sleep = 0;
unsigned long animationmode_sleep = 0;

uint8_t state = 0;
String animation_state = "";
bool state_first_run = true;
bool brightness_changed = false;
uint32_t random_color;
int createRandomColor_helper;

int color_circle_mode_helper = 0;
int color_pulse_helper_brightness = 255;
bool color_pulse_helper_lighten = true;
uint32_t mix_mode_helper = 0;
uint8_t learning_mode_helper = 0;
uint32_t rainbow_mode_helper = 0;

const char *input_sleep_time = "input_sleep_time";
const char *input_wakeup_time = "input_wakeup_time";
const char *input_animation_time = "input_animation_time";
const char *input_animation = "input_animation";
const char *input_timezone = "input_timezone";
const char *input_brightness = "input_brightness";

Clocktime user_wakeup_time;
Clocktime user_sleep_time;
Clocktime user_animation_time;
Clocktime current_time;
LampHelper helper;

/************************************************************************************************************
/*
/* Header
/*
*************/
void stateMachine();
void animationStateMachine();

void updateState(int new_state);
void changeAnimationState(String new_state);

void run_wakeupTime_mode();
void run_sleepingTime_mode();
void run_learning_mode();

void run_animation_mixed();
void run_circle();
void run_pulse();
void run_rainbow();
void run_lamp_off();

void updateStateAndTime();

void setNoneSleepingDelay(unsigned long wait, unsigned long *sleepUntilTime);
bool isSleeping();
void createRandomColor();

bool colorCircle(unsigned long wait);
bool colorPulse(unsigned long wait);
bool rainbowCircle(int wait);

void initTime();

void updateAnimation();
void updateBrightness();
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
    if(MDNS.begin("kinderlampe")) { // mDNS: kinderlampe.local
        Serial.println("mDNS responder started");
    } else {
        Serial.println("Error setting up MDNS responder!");
    }
    MDNS.addService("http", "tcp", 80);
    server.on("/", HTTP_GET, handle_server_root);
    server.on("/get", HTTP_GET, handle_server_get);
    server.onNotFound(handle_server_notFound);
    server.begin(); // Actually start the server
    // printServerInfo();

    // Load values from persistent storage or use default
    updateBrightness();
    updateAnimation();
    updateUserTimes();
    updateStateAndTime();
}

void loop() {
    MDNS.update();
    stateMachine();
    updateStateAndTime();
    // current_time.print();
}

/************************************************************************************************************
/*
/* Modes
/*
*************/
void run_animation_mixed() {
    if(state_first_run) { mix_mode_helper++; }
    if(mix_mode_helper >= 65) { mix_mode_helper = 0; }
    if(mix_mode_helper <= 40) {
        run_circle();
    } else if(mix_mode_helper <= 47) {
        run_pulse();
    } else {
        run_rainbow();
    }
}

void run_pulse() {
    if(state_first_run) {
        createRandomColor();
        color_pulse_helper_brightness = 2;
        color_pulse_helper_lighten = true;
        strip.fill(random_color);
        strip.setBrightness(color_pulse_helper_brightness);
        strip.show();
        Serial.println("run_pulse");
        state_first_run = false;
    }
    if(colorPulse(17)) { state_first_run = true; }
}

void run_circle() {
    if(state_first_run) {
        createRandomColor();
        state_first_run = false;
        Serial.println("run_circle");
    }
    if(colorCircle(100)) { state_first_run = true; }
}

void run_rainbow() {
    if(state_first_run) {
        rainbow_mode_helper = 0;
        state_first_run = false;
        Serial.println("run_rainbow");
    }
    if(rainbowCircle(20)) { state_first_run = true; }
}

void run_lamp_off() {
    if(state_first_run) {
        Serial.println("run_lamp_off");
        strip.fill(strip.Color(0, 0, 0, 0));
        strip.show();
        state_first_run = false;
    }
}

void run_wakeupTime_mode() {
    if(state_first_run || brightness_changed) {
        Serial.println("run_wakeupTime_mode");
        strip.fill(strip.Color(0, 75, 0, 0));
        strip.setPixelColor(3, strip.Color(0, 255, 0, 0));
        strip.setPixelColor(4, strip.Color(0, 255, 0, 0));
        strip.setPixelColor(5, strip.Color(0, 255, 0, 0));
        strip.setPixelColor(6, strip.Color(0, 255, 0, 0));
        strip.setPixelColor(7, strip.Color(0, 255, 0, 0));
        strip.setPixelColor(8, strip.Color(0, 255, 0, 0));
        strip.setBrightness(colorBrightness);
        strip.show();
        state_first_run = false;
        brightness_changed = false;
    }
}

void run_sleepingTime_mode() {
    if(state_first_run || brightness_changed) {
        Serial.println("run_sleepingTime_mode");
        strip.fill(strip.Color(255, 75, 0, 255));
        strip.setPixelColor(3, strip.Color(255, 9, 0, 0));
        strip.setPixelColor(4, strip.Color(255, 18, 0, 0));
        strip.setPixelColor(5, strip.Color(255, 37, 0, 0));
        strip.setPixelColor(6, strip.Color(255, 0, 0, 0));
        strip.setPixelColor(7, strip.Color(255, 0, 0, 0));
        strip.setPixelColor(8, strip.Color(255, 0, 0, 0));
        strip.setBrightness(colorBrightness);
        strip.show();
        state_first_run = false;
        brightness_changed = false;
    }
}

void run_learning_mode() {
    if(learning_mode_helper == 0) {
        run_wakeupTime_mode();
    } else {
        run_sleepingTime_mode();
    }
    if(isSleeping(animationmode_sleep)) { return; }
    if(learning_mode_helper == 0) {
        state_first_run = true;
        learning_mode_helper = 1;
    } else {
        state_first_run = true;
        learning_mode_helper = 0;
    }
    setNoneSleepingDelay(3000, &animationmode_sleep);
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
    } else if(state == STATE_ANIMATION_TIME) {
        animationStateMachine();
    } else {
        strip.fill(strip.Color(255, 128, 0, 255));
        strip.show();
    }
}

void animationStateMachine() {
    if(animation_state == STATE_ANIMATION_CIRCLE_PULSE) {
        run_animation_mixed();
    } else if(animation_state == STATE_ANIMATION_PULSE) {
        run_pulse();
    } else if(animation_state == STATE_ANIMATION_CIRCLE) {
        run_circle();
    } else if(animation_state == STATE_ANIMATION_RAINBOW) {
        run_rainbow();
    } else if(animation_state == STATE_ANIMATION_GREEN) {
        run_wakeupTime_mode();
    } else if(animation_state == STATE_ANIMATION_OFF) {
        run_lamp_off();
    } else if(animation_state == STATE_LEARNING) {
        run_learning_mode();
    } else {
        strip.fill(strip.Color(0, 0, 128, 255));
        strip.show();
    }
}

/************************************************************************************************************
/*
/* HELPER
/*
*************/

void updateStateAndTime() {
    updateTime();
    updateState(helper.get_state(current_time, user_animation_time,
                                 STATE_ANIMATION_TIME, user_sleep_time,
                                 STATE_SLEEPING_TIME, user_wakeup_time,
                                 STATE_WAKEUP_TIME));
}

String read_file(fs::FS &fs, const char *path) {
    File file = fs.open(path, "r");
    if(!file || file.isDirectory()) { return String(); }
    String fileContent;
    while(file.available()) {
        fileContent += String((char)file.read());
    }
    file.close();
    return fileContent;
}

void write_file(fs::FS &fs, const char *path, const char *message) {
    File file = fs.open(path, "w");
    if(!file) { return; }
    file.print(message);
    file.close();
}

void updateUserTimes() {
    // Wakeup Time
    String tmp_time = read_file(SPIFFS, "/input_wakeup_time.txt");
    if(tmp_time == "" || tmp_time == NULL) {
        tmp_time = "8:00";
        write_file(SPIFFS, "/input_wakeup_time.txt", tmp_time.c_str());
    }
    user_wakeup_time.setTime(tmp_time);
    user_wakeup_time.print();
    // Sleep Time
    tmp_time = read_file(SPIFFS, "/input_sleep_time.txt");
    if(tmp_time == "" || tmp_time == NULL) {
        tmp_time = "19:00";
        write_file(SPIFFS, "/input_sleep_time.txt", tmp_time.c_str());
    }
    user_sleep_time.setTime(tmp_time);
    user_sleep_time.print();
    // Animation Time
    tmp_time = read_file(SPIFFS, "/input_animation_time.txt");
    if(tmp_time == "" || tmp_time == NULL) {
        tmp_time = "8:30";
        write_file(SPIFFS, "/input_animation_time.txt", tmp_time.c_str());
    }
    user_animation_time.setTime(tmp_time);
    user_animation_time.print();
}

void updateBrightness() {
    String value = read_file(SPIFFS, "/input_brightness.txt");
    if(value == "" || value == NULL) {
        value = "75";
        write_file(SPIFFS, "/input_brightness.txt", value.c_str());
    }
    float percent = value.toFloat() / 100.0;
    if(percent > 1.0) { percent = 1.0; }
    brightness_changed = true;
    colorBrightness = (uint8_t)(255 * percent);
}

void updateAnimation() {
    String value = read_file(SPIFFS, "/input_animation.txt");
    if(value == "" || value == NULL) {
        value = "mix";
        write_file(SPIFFS, "/input_animation.txt", value.c_str());
    }
    for(int i = 0; i < sizeof(array_of_modes) / sizeof(array_of_modes[0]);
        i++) {
        if(value == array_of_modes[i][1]) {
            changeAnimationState(array_of_modes[i][1]);
            return;
        }
    }
}

void updateTimeZone() {
    String value = read_file(SPIFFS, "/input_timezone.txt");
    if(value == "" || value == NULL) {
        value = "Berlin";
        write_file(SPIFFS, "/input_timezone.txt", value.c_str());
    };
    for(int i = 0;
        i < sizeof(array_of_timezones) / sizeof(array_of_timezones[0]); i++) {
        if(value == array_of_timezones[i][0]) {
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
        if(value == "" || value == NULL) { value = "mix"; };
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
        if(value == "" || value == NULL) { value = "Europe_Berlin"; };
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

void async_wlan_setup() {
    AsyncWiFiManager wifiManager(&server, &dns);
    // reset saved settings >> USED TO TEST
    // wifiManager.resetSettings();
    wifiManager.autoConnect("Kinder Lampe");
}

void updateTime() {
    if(isSleeping(clock_sleep)) { return; }
    if(!getLocalTime(&timeinfo)) { return; }
    current_time.setTime(timeinfo.tm_hour, timeinfo.tm_min);
    setNoneSleepingDelay(200, &clock_sleep);
}

void initTime() {
    configTime(0, 0, "pool.ntp.org");
    updateTime();
    // Now we can set the real timezone
    // https://randomnerdtutorials.com/esp32-ntp-timezones-daylight-saving/
    // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
    updateTimeZone();
}

void setNoneSleepingDelay(unsigned long wait, unsigned long *time) {
    *time = millis() + wait;
}

bool isSleeping(unsigned long sleepUntilTime) {
    if(millis() < sleepUntilTime) { return true; }
    return false;
}

void changeAnimationState(String new_state) {
    animation_state = new_state;
    state_first_run = true;
}

void updateState(int new_state) {
    if(new_state == state) { return; }
    state = new_state;
    state_first_run = true;
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

/************************************************************************************************************
/*
/* Server Handler
/*
*************/
void printServerInfo() {
    Serial.print("Default hostname: ");
    Serial.println(WiFi.hostname().c_str());
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID()); // Tell us what network we're connected to
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());
}

void handle_server_root(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
}

void handle_server_get(AsyncWebServerRequest *request) {
    String tmp;
    if(request->hasParam(input_sleep_time)) {
        tmp = request->getParam(input_sleep_time)->value();
        if(user_sleep_time.setTime(tmp.c_str())) {
            write_file(SPIFFS, "/input_sleep_time.txt", tmp.c_str());
            updateStateAndTime();
        }
    }
    if(request->hasParam(input_wakeup_time)) {
        tmp = request->getParam(input_wakeup_time)->value();
        if(user_wakeup_time.setTime(tmp.c_str())) {
            write_file(SPIFFS, "/input_wakeup_time.txt", tmp.c_str());
            updateStateAndTime();
        }
    }
    if(request->hasParam(input_animation_time)) {
        tmp = request->getParam(input_animation_time)->value();
        if(user_animation_time.setTime(tmp.c_str())) {
            write_file(SPIFFS, "/input_animation_time.txt", tmp.c_str());
            updateStateAndTime();
        }
    }
    if(request->hasParam(input_animation)) {
        tmp = request->getParam(input_animation)->value();
        write_file(SPIFFS, "/input_animation.txt", tmp.c_str());
        updateAnimation();
    }
    if(request->hasParam(input_timezone)) {
        tmp = request->getParam(input_timezone)->value();
        write_file(SPIFFS, "/input_timezone.txt", tmp.c_str());
        updateTimeZone();
    }
    if(request->hasParam(input_brightness)) {
        tmp = request->getParam(input_brightness)->value();
        write_file(SPIFFS, "/input_brightness.txt", tmp.c_str());
        updateBrightness();
    }
    request->send(200, "text/text", "ok");
}

void handle_server_notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

/************************************************************************************************************
/*
/* Adafruit NeoPixel Standard Functions
/*
*************/

bool colorPulse(int wait) {
    if(isSleeping(animationmode_sleep)) { return false; }
    wait = (int)(wait * (255.0 / colorBrightness));
    if(color_pulse_helper_lighten) {
        color_pulse_helper_brightness++;
        if(color_pulse_helper_brightness >= colorBrightness) {
            color_pulse_helper_lighten = false;
        }
    } else {
        color_pulse_helper_brightness--;
        if(color_pulse_helper_brightness <= 2) { return true; }
    }
    strip.fill(random_color);
    strip.setBrightness(color_pulse_helper_brightness);
    strip.show();
    setNoneSleepingDelay(wait, &animationmode_sleep);
    return false;
}

bool colorCircle(int wait) {
    if(isSleeping(animationmode_sleep)) { return false; }
    if(color_circle_mode_helper >= strip.numPixels()) {
        color_circle_mode_helper = 0;
        return true;
    }
    strip.setPixelColor(color_circle_mode_helper, random_color);
    strip.setBrightness(colorBrightness);
    strip.show();
    color_circle_mode_helper++;
    setNoneSleepingDelay(wait, &animationmode_sleep);
    return false;
}

bool rainbowCircle(int wait) {
    if(isSleeping(animationmode_sleep)) { return false; }

    rainbow_mode_helper += 256;
    if(rainbow_mode_helper >= 65536) { return true; }
    for(int i = 0; i < strip.numPixels(); i++) {
        uint32_t pixelHue =
            rainbow_mode_helper + (i * 65536L / strip.numPixels());
        strip.setPixelColor(i,
                            strip.gamma32(strip.ColorHSV(pixelHue, 255, 255)));
    }
    strip.setBrightness(colorBrightness);
    strip.show();
    setNoneSleepingDelay(wait, &animationmode_sleep);
    return false;
}
