#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "secrets.h"
#include "time.h"
#include "aws_mqtt.h"

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

#define BUTTON_PIN 12
#define NEOPIXEL_PIN 4
#define NEOPIXEL_COUNT 16
#define STEPS 32  // 2^x
#define MULTIPLICATOR 256 / STEPS
#define ULONG_MAX (LONG_MAX * 2UL + 1UL)

struct tm timeinfo;

uint8_t colorBrightness = 64;  // Set BRIGHTNESS to about 1/5 (max = 255)
uint8_t lastColorBrightness = colorBrightness;

Adafruit_NeoPixel strip(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

unsigned long time_from_start = 0;
unsigned long sleep_till_time = 0;

uint8_t state = 0;
uint8_t last_state = 0;  // State which was choosen last
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

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

/************************************************************************************************************
/*
/* Header
/*
*************/
void test();
void handleInputs();
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

void handlePotiInput();
void handleTimeFromStart();
void handleButton();
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
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  // this resets all the neopixels to an off state
  strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();   // Turn OFF all pixels ASAP
  initTime();
}

void loop() {
  //aws_pubsub();
  handleInputs();
  stateMachine();
}

/************************************************************************************************************
/*
/* Test
/*
*************/

// ToDo: rainbowFade(3, 3);
void test() {
  for (int i = 0; i < strip.numPixels(); i++) {
    if (i > 10) {
      strip.setPixelColor(i, strip.Color(0, 128, 255, colorBrightness));  // Heaven_Blue
    } else {
      strip.setPixelColor(i, strip.Color(255, 0, 128, colorBrightness));  // Pink
    }
  }
  if (colorPulse(0, 5)) {
    // Back to start
  }
}


/************************************************************************************************************
/*
/* Modes
/*
*************/
void run_dayTime_mode_1_choosePulseOrWipe() {
  if (choose_pulse_wipe_counter < 100) {
    changeState(STATE_DAY_TIME_3_WIPE_CHOOSE_MIXED_OR_SINGLE_COLOR);
  } else {
    changeState(STATE_DAY_TIME_2_PULSE_CHOOSE_MIXED_OR_SINGLE_COLOR);
  }
  if (choose_pulse_wipe_counter > 110) { choose_pulse_wipe_counter = 0; }
  choose_pulse_wipe_counter++;
}

void run_dayTime_mode_2_Pulse_choose_mixed_or_single_color() {
  if (random(0, 2) == 0) {
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
  if (state_first_run) {
    createRandomColor();
    state_first_run = false;
  }
  if (colorPulse(random_color, 17)) {
    changeState(STATE_DAY_TIME_1_CHOOSE_PULSE_OR_WIPE);
  }
}

void run_dayTime_mode_3_Wipe_choose_mixed_or_single_color() {
  getBrightnessFromPoti();
  strip.setBrightness(colorBrightness);
  strip.show();
  if (random(0, 2) == 0) {
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
  if (state_first_run) {
    createRandomColor();
    state_first_run = false;
  }
  if (colorWipe(random_color, 100)) {
    changeState(STATE_DAY_TIME_1_CHOOSE_PULSE_OR_WIPE);
  }
}

void run_wakeupTime_mode() {
  if (state_first_run) {
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
  if (state_first_run) {
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
  if (!isNoneSleepingDelayOver()) { return; }
  if (learning_mode_substate == 0) {
    state_first_run = true;
    run_wakeupTime_mode();
    learning_mode_substate = 1;
  } else if (learning_mode_substate == 1) {
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

void handleInputs() {
  handlePotiInput();
  handleTimeFromStart();
  handleButton();
  if (state == STATE_LEARNING) {
    return;
  }
  // Time is also an input which gets handled if we are not in STATE_LEARNING
  handleDayTime();
}

void stateMachine() {
  if (state == STATE_SLEEPING_TIME) {
    run_sleepingTime_mode();
  } else if (state == STATE_WAKEUP_TIME) {
    run_wakeupTime_mode();
  } else if (state == STATE_DAY_TIME_1_CHOOSE_PULSE_OR_WIPE) {
    run_dayTime_mode_1_choosePulseOrWipe();
  } else if (state == STATE_DAY_TIME_2_PULSE_CHOOSE_MIXED_OR_SINGLE_COLOR) {
    run_dayTime_mode_2_Pulse_choose_mixed_or_single_color();
  } else if (state == STATE_DAY_TIME_2A_PULSE_CHOOSE_MIXED_COLOR) {
    run_dayTime_mode_2a_Pulse_mixed_color();
  } else if (state == STATE_DAY_TIME_2B_PULSE_CHOOSE_SINGLE_COLOR) {
    run_dayTime_mode_2b_Pulse_single_color();
  } else if (state == STATE_DAY_TIME_3_WIPE_CHOOSE_MIXED_OR_SINGLE_COLOR) {
    run_dayTime_mode_3_Wipe_choose_mixed_or_single_color();
  } else if (state == STATE_DAY_TIME_3A_WIPE_CHOOSE_MIXED_COLOR) {
    run_dayTime_mode_3a_Wipe_mixed_color();
  } else if (state == STATE_DAY_TIME_3B_WIPE_CHOOSE_SINGLE_COLOR) {
    run_dayTime_mode_3b_Wipe_single_color();
  } else if (state == STATE_LEARNING) {
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
void handlePotiInput() {
  getBrightnessFromPoti();
  if (colorBrightness < lastColorBrightness + 4 && colorBrightness > lastColorBrightness - 4) return;
  strip.setBrightness(colorBrightness);
  strip.show();
  lastColorBrightness = colorBrightness;
}

void handleTimeFromStart() {
  time_from_start = millis();
}

void handleButton() {
  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        last_state = state;
        state = STATE_LEARNING;
      } else {
        state = last_state;
      }
    }
  }
  lastButtonState = reading;
}

void handleDayTime() {
  if (!isNoneSleepingDelayOver()) { return; }
  updateTime();
  int h = timeinfo.tm_hour;
  int m = timeinfo.tm_min;
  Serial.print(h);
  Serial.print(":");
  Serial.println(m);
  if (h >= 18) {  // Schlafen 19:00 - 24:00 Uhr
    changeState(STATE_SLEEPING_TIME);
  } else if (h >= 10) {
    if (state == STATE_DAY_TIME_1_CHOOSE_PULSE_OR_WIPE
        || state == STATE_DAY_TIME_2_PULSE_CHOOSE_MIXED_OR_SINGLE_COLOR
        || state == STATE_DAY_TIME_2A_PULSE_CHOOSE_MIXED_COLOR
        || state == STATE_DAY_TIME_2B_PULSE_CHOOSE_SINGLE_COLOR
        || state == STATE_DAY_TIME_3_WIPE_CHOOSE_MIXED_OR_SINGLE_COLOR
        || state == STATE_DAY_TIME_3A_WIPE_CHOOSE_MIXED_COLOR
        || state == STATE_DAY_TIME_3B_WIPE_CHOOSE_SINGLE_COLOR) {
      return;  // only start mode once
    }
    changeState(STATE_DAY_TIME_1_CHOOSE_PULSE_OR_WIPE);
  } else if (h > 6 || (h == 6 && m >= 45)) {  // Gleich Zeit zum Aufstehen 06:45 - 10:00 Uhr
    changeState(STATE_WAKEUP_TIME);
  } else if (h < 6 || (h == 6 && m < 45)) {  // Schlafen 0:00 - 6:45 Uhr
    changeState(STATE_SLEEPING_TIME);
  }
  setNoneSleepingDelay(1000);
}


/************************************************************************************************************
/*
/* HELPER
/*
*************/

void updateTime(){
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time 1");
    return;
  }
}

void initTime(){
  configTime(0, 0, "pool.ntp.org");    // First connect to NTP server, with 0 TZ offset
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println("Got the time from NTP");
  // Now we can set the real timezone
  //https://randomnerdtutorials.com/esp32-ntp-timezones-daylight-saving/
  //https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
  setenv("TZ","CET-1CEST,M3.5.0,M10.5.0/3",1);
  tzset();
}

void setNoneSleepingDelay(unsigned long sleepTime) {
  sleep_till_time = time_from_start + sleepTime;
}

bool isNoneSleepingDelayOver() {
  if (time_from_start < sleep_till_time) {
    return false;
  }
  return true;
}

void changeState(int new_state) {
  if (last_state != STATE_LEARNING) {
    last_state = state;
  }
  state = new_state;
  state_first_run = true;
}
void getBrightnessFromPoti() {
  // Values from 0-255
  int a0 = analogRead(A0);
  if (a0 < STEPS) {
    colorBrightness = 1;
  } else {
    colorBrightness = (int(a0 / STEPS) * MULTIPLICATOR) - 1;
  }
  // printAnalog(a0);
}


void createRandomColor() {
  int r1 = random(0, 4);
  int r2 = random(0, 2);
  while (r2 == createRandomColor_helper) {
    r2 = random(0, 2);
  }
  createRandomColor_helper = r2;
  if (r1 == 0) {
    if (createRandomColor_helper == 0) {
      random_color = strip.Color(random(150, 256), random(0, 150), 0);
    } else {
      random_color = strip.Color(random(150, 256), 0, random(0, 150));
    }
  } else if (r1 == 1) {
    if (createRandomColor_helper == 0) {
      random_color = strip.Color(0, random(150, 256), random(0, 150));
    } else {
      random_color = strip.Color(random(0, 150), random(150, 256), 0);
    }
  } else {
    if (createRandomColor_helper == 0) {
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
  if (!isNoneSleepingDelayOver()) { return false; }
  if (colorWipe_helper_i >= strip.numPixels()) {
    colorWipe_helper_i = 0;
    return true;
  }
  strip.setPixelColor(colorWipe_helper_i, color);  //  Set pixel's color (in RAM)
  strip.show();                                    //  Update strip to match
  colorWipe_helper_i++;
  setNoneSleepingDelay(wait);
  return false;
}

bool colorPulse(uint32_t color, unsigned long wait) {
  if (!isNoneSleepingDelayOver()) { return false; }
  if (color > 0) {
    strip.fill(color);
    strip.show();
  }
  if (colorPulse_helper_i > 12) {
    colorPulse_helper_i--;
    strip.setBrightness(colorPulse_helper_i);
  } else if (colorPulse_helper_j < 255) {
    colorPulse_helper_j++;
    strip.setBrightness(colorPulse_helper_j);
  } else if (colorPulse_helper_k > 12) {
    colorPulse_helper_k--;
    strip.setBrightness(colorPulse_helper_k);
    if (colorPulse_helper_k == 13) {
      return true;
    }
  } else {
    colorPulse_helper_l++;
    strip.setBrightness(colorPulse_helper_l);
    if (colorPulse_helper_l >= 255) {
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
