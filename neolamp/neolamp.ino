#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define TESTMODE true

#define STATE_SLEEPING_TIME 0
#define STATE_WAKEUP_TIME 1
#define STATE_DAY_TIME 2
#define STATE_LEARNING 3

#define BUTTON_PIN      12
#define NEOPIXEL_PIN    4
#define NEOPIXEL_COUNT  16
#define STEPS 32 // 2^x
#define MULTIPLICATOR 256 / STEPS
#define ULONG_MAX (LONG_MAX * 2UL + 1UL)

const char *ssid     = "KOENIG";
const char *password = "Lachen*Lustig-Johanna";

const long utcOffsetInSeconds = 3600;
uint8_t colorBrightness = 64; // Set BRIGHTNESS to about 1/5 (max = 255)
uint8_t lastColorBrightness = colorBrightness;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

Adafruit_NeoPixel strip(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

unsigned long time_from_start = 0;
unsigned long sleep_till_time = 0;

uint8_t red = 0;
uint8_t blue = 0;
uint8_t green = 0;

uint8_t state = 0;
uint8_t last_state = 0;         // State which was choosen last
bool state_first_run = true;
uint8_t learning_mode_substate = 0;

int buttonState = false;
int lastButtonState = LOW;  
int colorWipe_i = 0;
uint8_t colorWipe_state = 0;

int pulse_i_counter = 255;
uint8_t pulse_state = 0;
int bla_pulse_j = 0;


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

void run_dayTime_mode();
void run_wakeupTime_mode();
void run_sleepingTime_mode();
void run_learning_mode();
void run_splashscreen_mode();

void handlePotiInput();
void handleTimeFromStart();
void handleButton();
void handleDayTime();

bool setNoneSleepingDelay(unsigned long  sleepTime);
bool isNoneSleepingDelayOver();
void changeState(int new_state);
void setColor_Changeover();
void getBrightnessFromPoti();
void printTime();
void printAnalog(int a0);

bool colorWipe(uint32_t color, int wait);
bool pulse(uint32_t color, int wait);
/************************************************************************************************************
/*
/* Arduino Functions 
/*
*************/

void setup(){
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  timeClient.begin();
  // this resets all the neopixels to an off state
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
}

void loop() {
  handleInputs();
  if(TESTMODE) {
    test();
  } else {
    stateMachine();
  }
}


/************************************************************************************************************
/*
/* Test
/*
*************/


  // ToDo: pulse(0,255,0,5);
  // ToDo: rainbowFade(3, 3);

  
  // ToDo: make it none sleeping
  // colorWipe(strip.Color(  255,   0,   0)     , 50); // Red
  // colorWipe(strip.Color(  255,   128, 0)     , 50); // Orange
  // colorWipe(strip.Color(  255,   255, 0)     , 50); // Yellow
  // colorWipe(strip.Color(  128,   255, 0)     , 50); // Liht-Green
  // colorWipe(strip.Color(  0, 255,   0)     , 50); // Green
  // colorWipe(strip.Color(  0,   255, 128)     , 50); // Türkis
  // colorWipe(strip.Color(  0,   255, 255)     , 50); // Light Blue
  // colorWipe(strip.Color(  0,   0, 255)     , 50); // Blue
  // colorWipe(strip.Color(  128,   0, 255)     , 50); // 
  // colorWipe(strip.Color(  255,   0, 255)     , 50); // Violet
  // colorWipe(strip.Color(  255,   0, 128)     , 50); // Pink

void test() { 
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    if(i>10) {
      strip.setPixelColor(i, strip.Color(0, 128, 255, colorBrightness)); // Heaven_Blue
    } else {
      strip.setPixelColor(i, strip.Color(255, 0, 128, colorBrightness));  // Pink
    }                          //  Pause for a moment
  }
  strip.show();
    if(pulse_state == 0) {
    if(pulse(0 , 50)){
      pulse_state++;
    } // Heaven-Blue
  } else if(pulse_state == 1) {
    if(pulse(strip.Color(  255,   150, 0)     , 50)) { // Pink
        pulse_state= 0;
      }
  }
   
  state_first_run =  false;
}


/************************************************************************************************************
/*
/* Modes
/*
*************/
void run_dayTime_mode() {
  if(colorWipe_state == 0) {
    if(colorWipe(strip.Color(  0,   128, 255)     , 50)){
      colorWipe_state++;
    } // Heaven-Blue
  } else if(colorWipe_state == 1) {
    if(colorWipe(strip.Color(  255,   0, 128)     , 50)) { // Pink
        colorWipe_state= 0;
      }
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
  if(!isNoneSleepingDelayOver()) { return; }
  if(learning_mode_substate == 0) {
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
  if(state == STATE_LEARNING) {
    return;
  }
  // Time is also an input which gets handled if we are not in STATE_LEARNING
  handleDayTime();
}

void stateMachine() {
  if(state == STATE_SLEEPING_TIME) {
    run_sleepingTime_mode();
  } else if(state == STATE_WAKEUP_TIME) {
    run_wakeupTime_mode();    
  } else if(state == STATE_DAY_TIME) {
    run_dayTime_mode();
  } else if(state == STATE_LEARNING) {
    run_learning_mode();
  } else {
    changeState(STATE_DAY_TIME); 
  }
}


/************************************************************************************************************
/*
/* Inputs
/*
*************/
void handlePotiInput() {
  getBrightnessFromPoti();
  if(colorBrightness  < lastColorBrightness + 4 &&
     colorBrightness > lastColorBrightness - 4) return;
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

void handleDayTime( ){
  timeClient.update();  
  int h = timeClient.getHours();
  int m = timeClient.getMinutes();
  if(h >= 19 && state != STATE_SLEEPING_TIME) {     // Schlafen 19:00 - 24:00 Uhr
    changeState(STATE_SLEEPING_TIME); 
  } else if(h >=10 && state != STATE_DAY_TIME) { // Tag 10:00 - 18:00 Uhr
    changeState(STATE_DAY_TIME); 
  } else if(h >= 6 && m >= 45 && state != STATE_WAKEUP_TIME) { // Gleich Zeit zum Aufstehen 07:00 - 10:00 Uhr
    changeState(STATE_WAKEUP_TIME); 
  } else if (state != STATE_SLEEPING_TIME) {     // Schlafen 0:00 - 6:45 Uhr
    changeState(STATE_SLEEPING_TIME); 
  }
}


/************************************************************************************************************
/*
/* HELPER
/*
*************/

bool setNoneSleepingDelay(unsigned long sleepTime) {
    sleep_till_time = time_from_start + sleepTime;
}

bool isNoneSleepingDelayOver() {
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

void setColor_Changeover() {
  strip.fill(strip.Color(255, 0, 128, colorBrightness));
  strip.show();
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

void printTime() {
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  //Serial.println(timeClient.getFormattedTime());
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
  if(!isNoneSleepingDelayOver()) { return false; }
  if(colorWipe_i >= strip.numPixels()) {
    colorWipe_i = 0;
    return true;
  }
  strip.setPixelColor(colorWipe_i, color);         //  Set pixel's color (in RAM)
  strip.show();                          //  Update strip to match
  colorWipe_i++;
  setNoneSleepingDelay(wait);
  return false;
}

bool pulse(uint32_t color, uint8_t wait) {
  if(!isNoneSleepingDelayOver()) { return false; }
  if(color != 0) {
    strip.fill(color);
  }
  if(pulse_i_counter > 0) {
    pulse_i_counter--;
    strip.setBrightness(pulse_i_counter);
  } else {
    bla_pulse_j++;
    strip.setBrightness(bla_pulse_j);
    if(bla_pulse_j >= 255) {
      pulse_i_counter = 255;
      bla_pulse_j = 0;
      return true;
    }
  }
  strip.show(); 
  setNoneSleepingDelay(wait);
  return false;
}


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

