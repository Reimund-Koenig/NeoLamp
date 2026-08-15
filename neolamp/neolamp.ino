#include "neolamp.h"

Adafruit_NeoPixel *strip;
LampFileSystem *lfs;
LampHelper helper;

uint8_t brightness = 100;
uint8_t colorBrightness = 0; // (max = 255)

unsigned long colorPicker_Color = 0;
bool isColorUpdateNeeded = true;

unsigned long clock_sleep = 0;
unsigned long substate_sleep = 0;

String state = STATE_DAYTIME;
bool state_first_run = true;
bool brightness_changed = false;
uint32_t random_color;
int createRandomColor_helper;

int last_a0;
unsigned long debounceDelay = 50; // the debounce time

#define LAMP_MODE_FS "/lamp_mode.txt"
#define LAMP_ON_FS "/lamp_on.txt"
#define LAMP_BRIGHTNESS_FS "/lamp_brightness.txt"

bool lampOn = true;
uint8_t currentModeIndex = 0;
uint8_t lampBrightnessPercent = 100;

int rawButtonState = HIGH;
int lastRawButtonState = HIGH;
bool stableSwitchOn = false;
bool lastStableSwitchOn = false;
unsigned long lastRawButtonChangeMs = 0;
unsigned long lastStableSwitchChangeMs = 0;

int color_circle_mode_helper = 0;
int color_circle_filled_mode_helper = 0;
int color_pulse_helper_brightness = 255;
bool color_pulse_helper_lighten = true;
uint32_t mix_mode_helper = 0;
uint32_t rainbow_mode_helper = 0;
uint8_t mixed_mode_index = 0;
unsigned long mixed_next_switch_ms = 0;
unsigned long mixed_hold_until_ms = 0;
bool mixed_hold_active = false;

uint8_t SETTING_NEOPIXEL_PIN;
uint8_t SETTING_NEOPIXEL_COUNT;

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
    lfs = new LampFileSystem();
    initSettings();
    strip = new Adafruit_NeoPixel(SETTING_NEOPIXEL_COUNT, SETTING_NEOPIXEL_PIN,
                                  NEO_GRB + NEO_KHZ800);

    strip->begin();
    strip->clear();
    strip->show();

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    int a0 = analogRead(A0);
    last_a0 = a0 - (a0 % STEPS);

    rawButtonState = digitalRead(BUTTON_PIN);
    lastRawButtonState = rawButtonState;
    stableSwitchOn = (rawButtonState == LOW);
    lastStableSwitchOn = stableSwitchOn;
    lastRawButtonChangeMs = millis();
    lastStableSwitchChangeMs = millis();

    loadLampState();
    renderLamp();
}

void loop() {
    handlePotiBrightnessInput();
    handleButton();
    stateMachine();
}

/************************************************************************************************************
/*
/* Modes
/*
*************/
void run_mixed() {
    if(state_first_run) {
        createRandomColor();
        mixed_mode_index = random(0, 3);
        mixed_next_switch_ms = millis() + random(5000, 15001);
        color_circle_mode_helper = 0;
        color_circle_filled_mode_helper = 0;
        rainbow_mode_helper = 0;
        state_first_run = false;
        Serial.println("run_mixed start");
    }

    if(millis() >= mixed_next_switch_ms) {
        createRandomColor();
        mixed_mode_index = random(0, 3);
        mixed_next_switch_ms = millis() + random(5000, 15001);
        color_circle_mode_helper = 0;
        color_circle_filled_mode_helper = 0;
        rainbow_mode_helper = 0;
        Serial.println("run_mixed switch");
    }

    switch(mixed_mode_index) {
    case 0:
        if(colorCircle(100)) { color_circle_mode_helper = 0; }
        break;
    case 1:
        if(colorCircleFilled(40)) { color_circle_filled_mode_helper = 0; }
        break;
    case 2:
        if(rainbowCircle(20)) { rainbow_mode_helper = 0; }
        break;
    default:
        setLampColorAndBrightness(getRgbColor(255, 0, 0),
                                  lampBrightnessPercent);
        break;
    }
}

void run_pulse() {
    if(state_first_run) {
        createRandomColor();
        color_pulse_helper_brightness = 2;
        color_pulse_helper_lighten = true;
        setLampColorAndBrightness(random_color, color_pulse_helper_brightness);
        Serial.println("run_pulse");
        state_first_run = false;
    }
    if(colorPulse(17)) { state_first_run = true; }
}

void run_circle() {
    if(state_first_run) {
        createRandomColor();
        color_circle_mode_helper = 0;
        state_first_run = false;
        Serial.println("run_circle");
    }
    if(colorCircle(150)) { state_first_run = true; }
}

void run_circle_filled() {
    if(state_first_run) {
        createRandomColor();
        color_circle_filled_mode_helper = 0;
        state_first_run = false;
        Serial.println("run_circle_filled");
    }
    if(colorCircleFilled(150)) { state_first_run = true; }
}

void run_rainbow() {
    if(state_first_run) {
        createRandomColor();
        rainbow_mode_helper = 0;
        state_first_run = false;
        Serial.println("run_rainbow");
    }
    if(rainbowCircle(20)) { state_first_run = true; }
}

void run_lamp_off() {
    if(state_first_run) {
        Serial.println("run_lamp_off");
        setLampColorAndBrightness(0, 0);
        state_first_run = false;
    }
}

/************************************************************************************************************
/*
/* Main Functions
/*
*************/

void stateMachine() {
    if(!lampOn) {
        strip->clear();
        strip->show();
        return;
    }

    colorBrightness = lampBrightnessPercent;

    switch(currentModeIndex) {
    case LAMP_MODE_MIX:
        animationStateMachine(STATE_ANIMATION_MIX);
        return;
    case LAMP_MODE_CIRCLE:
        animationStateMachine(STATE_ANIMATION_CIRCLE);
        return;
    case LAMP_MODE_CIRCLE_FILLED:
        animationStateMachine(STATE_ANIMATION_CIRCLE_FILLED);
        return;
    case LAMP_MODE_RAINBOW:
        animationStateMachine(STATE_ANIMATION_RAINBOW);
        return;
    default:
        renderLamp();
        return;
    }
}

void animationStateMachine(String substate) {
    if(substate == STATE_ANIMATION_MIX) {
        run_mixed();
    } else if(substate == STATE_ANIMATION_PULSE) {
        run_pulse();
    } else if(substate == STATE_ANIMATION_CIRCLE) {
        run_circle();
    } else if(substate == STATE_ANIMATION_CIRCLE_FILLED) {
        run_circle_filled();
    } else if(substate == STATE_ANIMATION_RAINBOW) {
        run_rainbow();
    } else if(substate == STATE_ANIMATION_OFF) {
        run_lamp_off();
    } else {
        setLampError();
    }
}

/************************************************************************************************************
/*
/* Inputs
/*
*************/
void handlePotiBrightnessInput() {
    int a0 = analogRead(A0);
    int percent = (int)((a0 / 1023.0) * 100.0);
    if(percent < 0) { percent = 0; }
    if(percent > 100) { percent = 100; }
    if(abs(percent - lampBrightnessPercent) < 1) { return; }
    lampBrightnessPercent = (uint8_t)percent;
    saveLampState();
}

void handleButton() {
    int reading = digitalRead(BUTTON_PIN);
    unsigned long now = millis();

    if(reading != lastRawButtonState) {
        lastRawButtonState = reading;
        lastRawButtonChangeMs = now;
    }

    if((now - lastRawButtonChangeMs) < debounceDelay) { return; }

    bool switchOn = (reading == LOW);
    if(switchOn != lastStableSwitchOn) {
        unsigned long delta = now - lastStableSwitchChangeMs;
        lastStableSwitchOn = switchOn;
        lastStableSwitchChangeMs = now;

        if(switchOn && !lampOn && delta <= 1000UL) {
            cycleMode();
            lampOn = true;
            saveLampState();
            return;
        }

        lampOn = switchOn;
        saveLampState();
    }
}

/************************************************************************************************************
/*
/* HELPER
/*
*************/
void initSettings() {
    String tmp = lfs->read_file(SETTING_NEOPIXEL_PIN_FS);
    if(tmp == "" || tmp == NULL) {
        SETTING_NEOPIXEL_PIN = NEOPIXEL_PIN;
        lfs->write_file(SETTING_NEOPIXEL_PIN_FS,
                        String(SETTING_NEOPIXEL_PIN).c_str());
    } else {
        SETTING_NEOPIXEL_PIN = (uint8_t)tmp.toInt();
    }
    tmp = lfs->read_file(SETTING_NEOPIXEL_COUNT_FS);
    if(tmp == "" || tmp == NULL) {
        SETTING_NEOPIXEL_COUNT = NEOPIXEL_COUNT;
        lfs->write_file(SETTING_NEOPIXEL_COUNT_FS,
                        String(SETTING_NEOPIXEL_COUNT).c_str());
    } else {
        SETTING_NEOPIXEL_COUNT = (uint8_t)tmp.toInt();
    }
}

void setLampError() { setLampColorAndBrightness(getRgbColor(0, 0, 128), 255); }

uint32_t getRgbColor(uint8_t r, uint8_t g, uint8_t b) {
    return strip->Color(r, g, b);
}

void setLampBrightness(uint8_t brightness) {
    if(brightness >= 9) {
        strip->setBrightness(brightness - 7);
        strip->show();
        return;
    }
    if(brightness == 0) {
        strip->setBrightness(brightness);
        strip->show();
        return;
    }
    strip->setBrightness(1);
    if(brightness <= 8) {
        strip->setPixelColor(7, 0);
        strip->setPixelColor(15, 0);
    }
    if(brightness <= 7) {
        strip->setPixelColor(5, 0);
        strip->setPixelColor(13, 0);
    }
    if(brightness <= 6) {
        strip->setPixelColor(3, 0);
        strip->setPixelColor(11, 0);
    }
    if(brightness <= 5) {
        strip->setPixelColor(1, 0);
        strip->setPixelColor(9, 0);
    }
    if(brightness <= 4) {
        strip->setPixelColor(6, 0);
        strip->setPixelColor(14, 0);
    }
    if(brightness <= 3) {
        strip->setPixelColor(2, 0);
        strip->setPixelColor(10, 0);
    }
    if(brightness <= 2) {
        strip->setPixelColor(4, 0);
        strip->setPixelColor(12, 0);
    }
    if(brightness == 1) {
        // only one LED on
        strip->setPixelColor(8, 0);
    }
    strip->show();
}

void setLampColorAndBrightness(uint32_t color, uint8_t brightness) {
    uint8_t percent = brightness > 100 ? (brightness * 100) / 255 : brightness;
    if(percent == 0) {
        strip->clear();
        strip->show();
        return;
    }
    strip->setBrightness((percent * 255) / 100);
    strip->fill(color);
    strip->show();
}

void saveLampState() {
    lfs->write_file(LAMP_BRIGHTNESS_FS, String(lampBrightnessPercent).c_str());
    lfs->write_file(LAMP_MODE_FS, String(currentModeIndex).c_str());
    lfs->write_file(LAMP_ON_FS, lampOn ? "1" : "0");
}

void loadLampState() {
    String value = lfs->read_file(LAMP_BRIGHTNESS_FS);
    if(value == "" || value == NULL) {
        lampBrightnessPercent = 100;
    } else {
        lampBrightnessPercent = constrain(value.toInt(), 0, 100);
    }

    value = lfs->read_file(LAMP_MODE_FS);
    if(value == "" || value == NULL) {
        currentModeIndex = 0;
    } else {
        currentModeIndex = constrain(value.toInt(), 0, MODE_COUNT - 1);
    }

    value = lfs->read_file(LAMP_ON_FS);
    lampOn = (value == "" || value == NULL || value == "1");
}

void cycleMode() {
    currentModeIndex = (currentModeIndex + 1) % MODE_COUNT;
    saveLampState();
}

void renderLamp() {
    switch(currentModeIndex) {
    case LAMP_MODE_RED:
        setLampColorAndBrightness(getRgbColor(255, 0, 0),
                                  lampBrightnessPercent);
        break;
    case LAMP_MODE_ORANGE:
        setLampColorAndBrightness(getRgbColor(255, 64, 0),
                                  lampBrightnessPercent);
        break;
    case LAMP_MODE_YELLOW:
        setLampColorAndBrightness(getRgbColor(255, 140, 0),
                                  lampBrightnessPercent);
        break;
    case LAMP_MODE_LIGHT_GREEN:
        setLampColorAndBrightness(getRgbColor(255, 255, 0),
                                  lampBrightnessPercent);
        break;
    case LAMP_MODE_GREEN:
        setLampColorAndBrightness(getRgbColor(0, 255, 0),
                                  lampBrightnessPercent);
        break;
    case LAMP_MODE_TURQUOISE:
        setLampColorAndBrightness(getRgbColor(0, 255, 255),
                                  lampBrightnessPercent);
        break;
    case LAMP_MODE_BLUE:
        setLampColorAndBrightness(getRgbColor(0, 0, 255),
                                  lampBrightnessPercent);
        break;
    case LAMP_MODE_PURPLE:
        setLampColorAndBrightness(getRgbColor(128, 0, 255),
                                  lampBrightnessPercent);
        break;
    case LAMP_MODE_MAGENTA:
        setLampColorAndBrightness(getRgbColor(255, 0, 255),
                                  lampBrightnessPercent);
        break;
    case LAMP_MODE_WHITE:
        setLampColorAndBrightness(getRgbColor(255, 255, 255),
                                  lampBrightnessPercent);
        break;
    case LAMP_MODE_MIX:
        run_mixed();
        break;
    case LAMP_MODE_CIRCLE:
        run_circle();
        break;
    case LAMP_MODE_CIRCLE_FILLED:
        run_circle_filled();
        break;
    case LAMP_MODE_RAINBOW:
        run_rainbow();
        break;
    default:
        setLampColorAndBrightness(getRgbColor(255, 0, 0),
                                  lampBrightnessPercent);
        break;
    }
}

void updateColorPicker(String state, const char *file) {
    (void)state;
    (void)file;
}

void updateColorBrightness(uint8_t inputBrightness) {
    if(colorBrightness == inputBrightness) { return; }
    brightness_changed = true;
    colorBrightness = inputBrightness;
}

uint8_t getLogicalBrightnessValue(String val) {
    float value = val.toFloat(); // Should 0-100
    // 0-20 will directly returned
    for(uint8_t i = 0; i < 20; i++) {
        if(value <= i) return value;
    }
    // value >= 20 from now on
    float percent = ((value - 20) * (100 / 80)) / 100;
    value = (235 * percent) + 20;
    return (uint8_t)(value);
}

void updateBrightness(String val) {
    brightness = getLogicalBrightnessValue(val);
    lampBrightnessPercent = brightness;
    lfs->write_file(LAMP_BRIGHTNESS_FS, String(brightness).c_str());
}

void updateState(String new_state) {
    if(state == new_state) { return; }
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
            random_color = getRgbColor(random(150, 256), random(0, 150), 0);
        } else {
            random_color = getRgbColor(random(150, 256), 0, random(0, 150));
        }
    } else if(r1 == 1) {
        if(createRandomColor_helper == 0) {
            random_color = getRgbColor(0, random(150, 256), random(0, 150));
        } else {
            random_color = getRgbColor(random(0, 150), random(150, 256), 0);
        }
    } else {
        if(createRandomColor_helper == 0) {
            random_color = getRgbColor(0, random(0, 150), random(30, 256));
        } else {
            random_color = getRgbColor(random(0, 150), 0, random(30, 256));
        }
    }
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
    setLampColorAndBrightness(random_color, color_pulse_helper_brightness);
    helper.set_none_sleeping_delay(wait, &substate_sleep);
    return false;
}

bool colorCircle(int wait) {
    if(helper.is_sleeping(substate_sleep)) { return false; }
    if(color_circle_mode_helper >= strip->numPixels()) {
        strip->clear();
        for(int i = 0; i < strip->numPixels(); i++) {
            strip->setPixelColor(i, random_color);
        }
        strip->setBrightness((lampBrightnessPercent * 255) / 100);
        strip->show();
        color_circle_mode_helper = 0;
        return true;
    }
    strip->clear();
    strip->setBrightness((lampBrightnessPercent * 255) / 100);
    int pixelIndex = strip->numPixels() - 1 - color_circle_mode_helper;
    strip->setPixelColor(pixelIndex, random_color);
    strip->show();
    color_circle_mode_helper++;
    helper.set_none_sleeping_delay(wait, &substate_sleep);
    return false;
}

bool colorCircleFilled(int wait) {
    if(helper.is_sleeping(substate_sleep)) { return false; }

    if(color_circle_filled_mode_helper >= strip->numPixels()) {
        for(int i = 0; i < strip->numPixels(); i++) {
            strip->setPixelColor(i, random_color);
        }
        strip->show();
        color_circle_filled_mode_helper = 0;
        return true;
    }

    for(int i = strip->numPixels() - 1;
        i >= strip->numPixels() - 1 - color_circle_filled_mode_helper; i--) {
        strip->setPixelColor(i, random_color);
    }
    setLampBrightness(colorBrightness);
    strip->show();
    color_circle_filled_mode_helper++;
    helper.set_none_sleeping_delay(wait, &substate_sleep);
    return false;
}

bool rainbowCircle(int wait) {
    if(helper.is_sleeping(substate_sleep)) { return false; }

    rainbow_mode_helper += 256;
    if(rainbow_mode_helper >= 65536) {
        rainbow_mode_helper = 0;
        return true;
    }

    for(int i = 0; i < strip->numPixels(); i++) {
        uint32_t pixelHue =
            rainbow_mode_helper + (i * 65536L / strip->numPixels());
        strip->setPixelColor(
            i, strip->gamma32(strip->ColorHSV(pixelHue, 255, 255)));
    }
    setLampBrightness(colorBrightness);
    helper.set_none_sleeping_delay(wait, &substate_sleep);
    return false;
}

/************************************************************************************************************
/*
/* HELPER  INITIALIZE
/*
*************/

void initBrightness() {
    String value = lfs->read_file(LAMP_BRIGHTNESS_FS);
    if(value == "" || value == NULL) { value = "100"; }
    updateBrightness(value);
}

void initColors() {
    // Intentionally kept for compatibility with the current color handling.
}

void initModes() {
    String value = lfs->read_file(LAMP_MODE_FS);
    if(value == "" || value == NULL) { value = String(currentModeIndex); }
    currentModeIndex = constrain(value.toInt(), 0, MODE_COUNT - 1);
    state = STATE_ANIMATION_MIX;
}
