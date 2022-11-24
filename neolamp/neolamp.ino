#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define SWITCH_PIN      12
#define NEOPIXEL_PIN    4
#define NEOPIXEL_COUNT  16
#define TESTMODE false
#define STEPS 32 // 2^x
#define MULTIPLICATOR 256 / STEPS

const char *ssid     = "KOENIG";
const char *password = "Lachen*Lustig-Johanna";

const long utcOffsetInSeconds = 3600;
uint8_t colorBrightness = 64; // Set BRIGHTNESS to about 1/5 (max = 255)

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

Adafruit_NeoPixel strip(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

uint8_t red = 0;
uint8_t blue = 0;
uint8_t green = 0;

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
  timeClient.begin();
  // this resets all the neopixels to an off state
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
}

void loop() {
  if(digitalRead(SWITCH_PIN)==HIGH){
    return;
  } 
  timeClient.update();
  getBrightnessFromPoti();
  strip.setBrightness(colorBrightness);
  if(TESTMODE) {
    doStuff();
  } else {
    dailyRoutine();
  }
}

/************************************************************************************************************
Functions
*/

void doStuff() {
  colorWipe(strip.Color(  255,   0,   0)     , 50); // Red
  colorWipe(strip.Color(  255,   128, 0)     , 50); // Orange
  colorWipe(strip.Color(  255,   255, 0)     , 50); // Yellow
  colorWipe(strip.Color(  128,   255, 0)     , 50); // Liht-Green
  colorWipe(strip.Color(  0, 255,   0)     , 50); // Green
  colorWipe(strip.Color(  0,   255, 128)     , 50); // Türkis
  colorWipe(strip.Color(  0,   255, 255)     , 50); // Light Blue
  colorWipe(strip.Color(  0,   128, 255)     , 50); // Heaven-Blue
  colorWipe(strip.Color(  0,   0, 255)     , 50); // Blue
  colorWipe(strip.Color(  128,   0, 255)     , 50); // 
  colorWipe(strip.Color(  255,   0, 255)     , 50); // Violet
  colorWipe(strip.Color(  255,   0, 128)     , 50); // Pink
  colorWipe(strip.Color(  128,   128, 128)     , 50); // Gray
  //pulse(0,255,0,5);
  //rainbowFade(3, 3);
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

void dailyRoutine() {
  int h = timeClient.getHours();
  int m = timeClient.getMinutes();
  if(h >= 19) {     // Schlafen 19:00 - 24:00 Uhr
    setColor_Go2Sleep(); 
  }
  else if(h >= 18) {     // Bettgeh fertig machen 18:00 - 19:00 Uhr
    setColor_Changeover(); 
  }
  else if(h >=10) { // Tag 10:00 - 18:00 Uhr
    doStuff();
  }
  else if(h >= 7) { // Zeit zum Aufstehen 07:00 - 10:00 Uhr
    setColor_WakeUp();
  }
  else if(h >= 6 && m >= 45) { // Gleich Zeit zum Aufstehen 07:00 - 10:00 Uhr
    setColor_Changeover();
  }
  else {     // Schlafen 0:00 - 6:45 Uhr
    setColor_Go2Sleep(); 
  }
  delay(200);
}

void setColor_WakeUp() {
  strip.fill(strip.Color(0, 255, 0, colorBrightness));
  strip.show();
}

void setColor_Go2Sleep() {
  strip.fill(strip.Color(255, 150, 0, colorBrightness));
  strip.show();
}

void setColor_Changeover(){
  strip.fill(strip.Color(255, 0, 128, colorBrightness));
  strip.show();
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
  delay(1000);
}

/************************************************************************************************************
Adafruit NeoPixel Standard Functions
*/

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void pulse(int red, int green, int blue, uint8_t wait) {
    strip.fill(strip.Color(red, green, blue, 255));
  for(int j=0; j<256; j++) { // Ramp up from 0 to 255
    // Fill entire strip with white at gamma-corrected brightness level 'j':
    strip.setBrightness(j);
    strip.show();
    delay(wait);
  }

  for(int j=255; j>=0; j--) { // Ramp down from 255 to 0
    strip.setBrightness(j);
    strip.show();
    delay(wait);
  }
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

