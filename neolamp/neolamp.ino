#include "neolamp.h"

AsyncWebServer server(80);
DNSServer dns;

Adafruit_NeoPixel strip(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

struct tm timeinfo;

uint8_t wakeup_brightness = 25;
uint8_t daytime_brightness = 100;
uint8_t sleep_brightness = 15;
uint8_t colorBrightness = 0; // (max = 255)

bool wakeup_isColorPickerNeeded = false;
bool daytime_isColorPickerNeeded = false;
bool sleep_isColorPickerNeeded = false;
unsigned long colorPicker_Color = 0;
bool isColorUpdateNeeded = true;

unsigned long clock_sleep = 0;
unsigned long substate_sleep = 0;

uint8_t state = 0;
String wakeup_state = "";
String sleep_state = "";
String daytime_state = "";
bool state_first_run = true;
bool brightness_changed = false;
uint32_t random_color;
int createRandomColor_helper;

int color_circle_mode_helper = 0;
int color_pulse_helper_brightness = 255;
bool color_pulse_helper_lighten = true;
uint32_t mix_mode_helper = 0;
uint32_t rainbow_mode_helper = 0;

Clocktime user_wakeup_time;
Clocktime user_sleep_time;
Clocktime user_daytime_time;
Clocktime current_time;
LampHelper helper;
Doubleblink *db;
LampFileSystem *lfs;

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
    // this resets all the neopixels to an off state
    strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.setBrightness(150);
    strip.fill(strip.Color(255, 255, 255));
    strip.show();
    lfs = new LampFileSystem();

    // start wifi manager
    WiFi.mode(WIFI_STA);
    WiFi.hostname(URL);
    async_wlan_setup();
    while(WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    initTime();
    if(MDNS.begin(URL)) { // browser: url.local
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
    db = new Doubleblink(lfs);

    update_wakeup_brightness();
    update_daytime_brightness();
    update_sleep_brightness();

    update_wakeup_mode();
    update_daytime_mode();
    update_sleep_mode();

    update_wakeup_color();
    update_daytime_color();
    update_sleep_color();

    updateUserTimes();
    updateStateAndTime();

    db->updateBlinkState(state);
}

void loop() {
    MDNS.update();
    stateMachine();
    // db->loop();
    updateStateAndTime();
}

/************************************************************************************************************
/*
/* Modes
/*
*************/
void run_mixed() {
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

void run_colorPick_mode() {
    if(state_first_run) {
        isColorUpdateNeeded = true;
        state_first_run = false;
        Serial.println("run_colorPick_mode");
    }
    if(!(isColorUpdateNeeded || brightness_changed)) { return; }
    for(int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, colorPicker_Color);
    }
    strip.setBrightness(colorBrightness);
    strip.show();
    isColorUpdateNeeded = false;
    brightness_changed = false;
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

/************************************************************************************************************
/*
/* Main Functions
/*
*************/

void stateMachine() {
    if(state == STATE_WAKEUP_TIME) {
        update_color_brightness(wakeup_brightness);
        update_color_picker(wakeup_state, DAYTIME_COLOR_FS);
        animationStateMachine(wakeup_state);
    } else if(state == STATE_DAYTIME_TIME) {
        update_color_brightness(daytime_brightness);
        update_color_picker(daytime_state, DAYTIME_COLOR_FS);
        animationStateMachine(daytime_state);
    } else if(state == STATE_SLEEPING_TIME) {
        update_color_brightness(sleep_brightness);
        update_color_picker(sleep_state, SLEEP_COLOR_FS);
        animationStateMachine(sleep_state);
    } else {
        strip.fill(strip.Color(255, 128, 0, 255));
        strip.show();
    }
}

void animationStateMachine(String substate) {
    if(substate == STATE_ANIMATION_MIX) {
        run_mixed();
    } else if(substate == STATE_ANIMATION_PULSE) {
        run_pulse();
    } else if(substate == STATE_ANIMATION_CIRCLE) {
        run_circle();
    } else if(substate == STATE_ANIMATION_RAINBOW) {
        run_rainbow();
    } else if(substate == STATE_ANIMATION_PICK) {
        run_colorPick_mode();
    } else if(substate == STATE_ANIMATION_GREEN) {
        run_wakeupTime_mode();
    } else if(substate == STATE_ANIMATION_RED) {
        run_sleepingTime_mode();
    } else if(substate == STATE_ANIMATION_OFF) {
        run_lamp_off();
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
    if(helper.is_sleeping(clock_sleep)) { return; }
    updateTime();
    updateState(helper.get_state(
        current_time, user_daytime_time, STATE_DAYTIME_TIME, user_sleep_time,
        STATE_SLEEPING_TIME, user_wakeup_time, STATE_WAKEUP_TIME));
    helper.set_none_sleeping_delay(200, &clock_sleep);
}

void updateUserTimes() {
    // Wakeup Time
    String tmp_time = lfs->read_file(WAKEUP_TIME_FS);
    if(tmp_time == "" || tmp_time == NULL) {
        tmp_time = "08:00";
        lfs->write_file(WAKEUP_TIME_FS, tmp_time.c_str());
    }
    user_wakeup_time.setTime(tmp_time);
    user_wakeup_time.print();
    // Daytime Time
    tmp_time = lfs->read_file(DAYTIME_TIME_FS);
    if(tmp_time == "" || tmp_time == NULL) {
        tmp_time = "08:30";
        lfs->write_file(DAYTIME_TIME_FS, tmp_time.c_str());
    }
    user_daytime_time.setTime(tmp_time);
    user_daytime_time.print();
    // Sleep Time
    tmp_time = lfs->read_file(SLEEP_TIME_FS);
    if(tmp_time == "" || tmp_time == NULL) {
        tmp_time = "19:00";
        lfs->write_file(SLEEP_TIME_FS, tmp_time.c_str());
    }
    user_sleep_time.setTime(tmp_time);
    user_sleep_time.print();
}

void update_color_picker(String state, const char *file) {
    if(!isColorUpdateNeeded) { return; }
    if(state != STATE_ANIMATION_PICK) { return; }
    String inputColor = lfs->read_file(file);
    inputColor.remove(0, 1);
    unsigned long in = strtoul(inputColor.c_str(), NULL, 16);
    Serial.print("INPUT: ");
    Serial.print(inputColor);
    Serial.print(", Long: ");
    Serial.println(in);
    colorPicker_Color = in;
}

void update_color_brightness(uint8_t inputBrightness) {
    if(colorBrightness == inputBrightness) { return; }
    brightness_changed = true;
    colorBrightness = inputBrightness;
}

void update_wakeup_brightness() {
    String value = lfs->read_file(WAKEUP_BRIGHTNESS_FS);
    if(value == "" || value == NULL) {
        value = "15";
        lfs->write_file(WAKEUP_BRIGHTNESS_FS, value.c_str());
    }
    float percent = value.toFloat() / 100.0;
    if(percent > 1.0) { percent = 1.0; }
    wakeup_brightness = (uint8_t)(255 * percent);
}

void update_daytime_brightness() {
    String value = lfs->read_file(DAYTIME_BRIGHTNESS_FS);
    if(value == "" || value == NULL) {
        value = "100";
        lfs->write_file(DAYTIME_BRIGHTNESS_FS, value.c_str());
    }
    float percent = value.toFloat() / 100.0;
    if(percent > 1.0) { percent = 1.0; }
    daytime_brightness = (uint8_t)(255 * percent);
}

void update_sleep_brightness() {
    String value = lfs->read_file(SLEEP_BRIGHTNESS_FS);
    if(value == "" || value == NULL) {
        value = "15";
        lfs->write_file(SLEEP_BRIGHTNESS_FS, value.c_str());
    }
    float percent = value.toFloat() / 100.0;
    if(percent > 1.0) { percent = 1.0; }
    sleep_brightness = (uint8_t)(255 * percent);
}

void update_wakeup_color() {
    String value = lfs->read_file(WAKEUP_COLOR_FS);
    if(value == "" || value == NULL) {
        value = "#90EE90"; // lightgreen
        lfs->write_file(WAKEUP_COLOR_FS, value.c_str());
    }
}

void update_daytime_color() {
    String value = lfs->read_file(DAYTIME_COLOR_FS);
    if(value == "" || value == NULL) {
        value = "#00FFFF"; // cyan
        lfs->write_file(DAYTIME_COLOR_FS, value.c_str());
    }
}

void update_sleep_color() {
    String value = lfs->read_file(SLEEP_COLOR_FS);
    if(value == "" || value == NULL) {
        value = "#FF8C00"; // dark orange
        lfs->write_file(SLEEP_COLOR_FS, value.c_str());
    }
}

void update_wakeup_mode() {
    String value = lfs->read_file(WAKEUP_MODE_FS);
    if(value == "" || value == NULL) {
        value = "green";
        lfs->write_file(WAKEUP_MODE_FS, value.c_str());
    }
    for(int i = 0; i < sizeof(modes) / sizeof(modes[0]); i++) {
        if(value == modes[i][1]) {
            change_wakeup_state(modes[i][1]);
            wakeup_isColorPickerNeeded = !(value == STATE_ANIMATION_PICK);
            return;
        }
    }
}

void update_daytime_mode() {
    String value = lfs->read_file(DAYTIME_MODE_FS);
    if(value == "" || value == NULL) {
        value = "mix";
        lfs->write_file(DAYTIME_MODE_FS, value.c_str());
    }
    for(int i = 0; i < sizeof(modes) / sizeof(modes[0]); i++) {
        if(value == modes[i][1]) {
            change_daytime_state(modes[i][1]);
            daytime_isColorPickerNeeded = !(value == STATE_ANIMATION_PICK);
            return;
        }
    }
}

void update_sleep_mode() {
    String value = lfs->read_file(SLEEP_MODE_FS);
    if(value == "" || value == NULL) {
        value = "orange";
        lfs->write_file(SLEEP_MODE_FS, value.c_str());
    }
    for(int i = 0; i < sizeof(modes) / sizeof(modes[0]); i++) {
        if(value == modes[i][1]) {
            change_sleep_state(modes[i][1]);
            sleep_isColorPickerNeeded = !(value == STATE_ANIMATION_PICK);
            return;
        }
    }
}

void updateTimeZone() {
    String value = lfs->read_file(TIMEZONE_FS);
    if(value == "" || value == NULL) {
        value = "Berlin";
        lfs->write_file(TIMEZONE_FS, value.c_str());
    };
    for(int i = 0; i < sizeof(timezones) / sizeof(timezones[0]); i++) {
        if(value == timezones[i][0]) {
            setenv("TZ", timezones[i][1], 1);
            tzset();
            updateTime();
            return;
        }
    }
}

String processor(const String &var) {
    if(var == "input_name") { return NAME; }

    // Times
    if(var == WAKEUP_TIME_IN) {
        return lfs->read_file(WAKEUP_TIME_FS);
    } else if(var == DAYTIME_TIME_IN) {
        return lfs->read_file(DAYTIME_TIME_FS);
    } else if(var == SLEEP_TIME_IN) {
        return lfs->read_file(SLEEP_TIME_FS);
    }
    // Modes
    else if(var == WAKEUP_MODE_IN) {
        String value = lfs->read_file(WAKEUP_MODE_FS);
        if(value == "" || value == NULL) { value = "green"; };
        return helper.getHtmlSelect(modes, sizeof_modes, value);
    } else if(var == DAYTIME_MODE_IN) {
        String value = lfs->read_file(DAYTIME_MODE_FS);
        if(value == "" || value == NULL) { value = "mix"; };
        return helper.getHtmlSelect(modes, sizeof_modes, value);
    } else if(var == SLEEP_MODE_IN) {
        String value = lfs->read_file(SLEEP_MODE_FS);
        if(value == "" || value == NULL) { value = "orange"; };
        return helper.getHtmlSelect(modes, sizeof_modes, value);
    }
    // Brightness
    else if(var == WAKEUP_BRIGHTNESS_IN) {
        return lfs->read_file(WAKEUP_BRIGHTNESS_FS);
    } else if(var == DAYTIME_BRIGHTNESS_IN) {
        return lfs->read_file(DAYTIME_BRIGHTNESS_FS);
    } else if(var == SLEEP_BRIGHTNESS_IN) {
        return lfs->read_file(SLEEP_BRIGHTNESS_FS);
    }
    // Blink Interval
    else if(var == BLINK_INTERVAL_IN) {
        return lfs->read_file(BLINK_INTERVAL_FS);
    }
    // Blink LEDs
    else if(var == WAKEUP_BLINK_IN) {
        String value = lfs->read_file(WAKEUP_BLINK_FS);
        if(value == "" || value == NULL) { value = D_LED_MODE_OFF; }
        return helper.getHtmlSelect(blink_modes, sizeof_blink_modes, value);
    } else if(var == DAYTIME_BLINK_IN) {
        String value = lfs->read_file(DAYTIME_BLINK_FS);
        if(value == "" || value == NULL) { value = D_LED_MODE_BLINK; }
        return helper.getHtmlSelect(blink_modes, sizeof_blink_modes, value);
    } else if(var == SLEEP_BLINK_IN) {
        String value = lfs->read_file(SLEEP_BLINK_FS);
        if(value == "" || value == NULL) { value = D_LED_MODE_BLINK; }
        return helper.getHtmlSelect(blink_modes, sizeof_blink_modes, value);
    }
    // Color
    else if(var == WAKEUP_COLOR_IN) {
        return lfs->read_file(WAKEUP_COLOR_FS);
    } else if(var == DAYTIME_COLOR_IN) {
        return lfs->read_file(DAYTIME_COLOR_FS);
    } else if(var == SLEEP_COLOR_IN) {
        return lfs->read_file(SLEEP_COLOR_FS);
    }
    // Color
    else if(var == WAKEUP_COLOR_ROW_IN) {
        if(wakeup_isColorPickerNeeded) { return "hidden"; }
        return "";
    } else if(var == DAYTIME_COLOR_ROW_IN) {
        if(daytime_isColorPickerNeeded) { return "hidden"; }
        return "";
    } else if(var == SLEEPTIME_COLOR_ROW_IN) {
        if(sleep_isColorPickerNeeded) { return "hidden"; }
        return "";
    }
    // The current time
    else if(var == "input_time_on_load") {
        updateTime();
        return current_time.getTimeString();
    }
    // Timezone
    else if(var == TIMEZONE_IN) {
        String value = lfs->read_file(TIMEZONE_FS);
        if(value == "" || value == NULL) { value = "Europe_Berlin"; };
        return helper.getHtmlSelect(timezones, sizeof_timezones, value);
    }
    return "";
}

void async_wlan_setup() {
    AsyncWiFiManager wifiManager(&server, &dns);
    // reset saved settings >> USED TO TEST
    // wifiManager.resetSettings();
    wifiManager.autoConnect(NAME);
}

void updateTime() {
    if(!getLocalTime(&timeinfo)) { return; }
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

void change_wakeup_state(String new_state) {
    if(new_state == wakeup_state) { return; }
    wakeup_state = new_state;
    state_first_run = true;
}

void change_daytime_state(String new_state) {
    if(new_state == daytime_state) { return; }
    daytime_state = new_state;
    state_first_run = true;
}

void change_sleep_state(String new_state) {
    if(new_state == sleep_state) { return; }
    sleep_state = new_state;
    state_first_run = true;
}
void updateState(int new_state) {
    if(state == new_state) { return; }
    state = new_state;
    state_first_run = true;
    db->updateBlinkState(state);
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
    // Times
    if(request->hasParam(WAKEUP_TIME_IN)) {
        tmp = request->getParam(WAKEUP_TIME_IN)->value();
        lfs->write_file(WAKEUP_TIME_FS, tmp.c_str());
        user_wakeup_time.setTime(tmp.c_str());
    } else if(request->hasParam(DAYTIME_TIME_IN)) {
        tmp = request->getParam(DAYTIME_TIME_IN)->value();
        lfs->write_file(DAYTIME_TIME_FS, tmp.c_str());
        user_daytime_time.setTime(tmp.c_str());
    } else if(request->hasParam(SLEEP_TIME_IN)) {
        tmp = request->getParam(SLEEP_TIME_IN)->value();
        lfs->write_file(SLEEP_TIME_FS, tmp.c_str());
        user_sleep_time.setTime(tmp.c_str());
    }
    // Modes
    else if(request->hasParam(WAKEUP_MODE_IN)) {
        tmp = request->getParam(WAKEUP_MODE_IN)->value();
        lfs->write_file(WAKEUP_MODE_FS, tmp.c_str());
        change_wakeup_state(tmp.c_str());
    } else if(request->hasParam(DAYTIME_MODE_IN)) {
        tmp = request->getParam(DAYTIME_MODE_IN)->value();
        lfs->write_file(DAYTIME_MODE_FS, tmp.c_str());
        change_daytime_state(tmp.c_str());
    } else if(request->hasParam(SLEEP_MODE_IN)) {
        tmp = request->getParam(SLEEP_MODE_IN)->value();
        lfs->write_file(SLEEP_MODE_FS, tmp.c_str());
        change_sleep_state(tmp.c_str());
    }
    // Brightness
    else if(request->hasParam(WAKEUP_BRIGHTNESS_IN)) {
        tmp = request->getParam(WAKEUP_BRIGHTNESS_IN)->value();
        lfs->write_file(WAKEUP_BRIGHTNESS_FS, tmp.c_str());
        update_wakeup_brightness();
    } else if(request->hasParam(DAYTIME_BRIGHTNESS_IN)) {
        tmp = request->getParam(DAYTIME_BRIGHTNESS_IN)->value();
        lfs->write_file(DAYTIME_BRIGHTNESS_FS, tmp.c_str());
        update_daytime_brightness();
    } else if(request->hasParam(SLEEP_BRIGHTNESS_IN)) {
        tmp = request->getParam(SLEEP_BRIGHTNESS_IN)->value();
        lfs->write_file(SLEEP_BRIGHTNESS_FS, tmp.c_str());
        update_sleep_brightness();
    }
    // Blink Interval
    else if(request->hasParam(BLINK_INTERVAL_IN)) {
        tmp = request->getParam(BLINK_INTERVAL_IN)->value();
        lfs->write_file(BLINK_INTERVAL_FS, tmp.c_str());
        db->set_interval((uint16_t)(tmp.toInt()));
    }
    // Blink LEDs
    else if(request->hasParam(WAKEUP_BLINK_IN)) {
        tmp = request->getParam(WAKEUP_BLINK_IN)->value();
        lfs->write_file(WAKEUP_BLINK_FS, tmp.c_str());
        db->updateBlinkState(state);
    } else if(request->hasParam(DAYTIME_BLINK_IN)) {
        tmp = request->getParam(DAYTIME_BLINK_IN)->value();
        lfs->write_file(DAYTIME_BLINK_FS, tmp.c_str());
        db->updateBlinkState(state);
    } else if(request->hasParam(SLEEP_BLINK_IN)) {
        tmp = request->getParam(SLEEP_BLINK_IN)->value();
        lfs->write_file(SLEEP_BLINK_FS, tmp.c_str());
        db->updateBlinkState(state);
    }
    // Color
    else if(request->hasParam(WAKEUP_COLOR_IN)) {
        tmp = request->getParam(WAKEUP_COLOR_IN)->value();
        lfs->write_file(WAKEUP_COLOR_FS, tmp.c_str());
        isColorUpdateNeeded = true;
    } else if(request->hasParam(DAYTIME_COLOR_IN)) {
        tmp = request->getParam(DAYTIME_COLOR_IN)->value();
        isColorUpdateNeeded = true;
        lfs->write_file(DAYTIME_COLOR_FS, tmp.c_str());
    } else if(request->hasParam(SLEEP_COLOR_IN)) {
        tmp = request->getParam(SLEEP_COLOR_IN)->value();
        lfs->write_file(SLEEP_COLOR_FS, tmp.c_str());
        isColorUpdateNeeded = true;
    }
    // Timezone
    else if(request->hasParam(TIMEZONE_IN)) {
        tmp = request->getParam(TIMEZONE_IN)->value();
        lfs->write_file(TIMEZONE_FS, tmp.c_str());
        updateTimeZone();
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
    if(helper.is_sleeping(substate_sleep)) { return false; }
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
    helper.set_none_sleeping_delay(wait, &substate_sleep);
    return false;
}

bool colorCircle(int wait) {
    if(helper.is_sleeping(substate_sleep)) { return false; }
    if(color_circle_mode_helper >= strip.numPixels()) {
        color_circle_mode_helper = 0;
        return true;
    }
    strip.setPixelColor(color_circle_mode_helper, random_color);
    strip.setBrightness(colorBrightness);
    strip.show();
    color_circle_mode_helper++;
    helper.set_none_sleeping_delay(wait, &substate_sleep);
    return false;
}

bool rainbowCircle(int wait) {
    if(helper.is_sleeping(substate_sleep)) { return false; }

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
    helper.set_none_sleeping_delay(wait, &substate_sleep);
    return false;
}
