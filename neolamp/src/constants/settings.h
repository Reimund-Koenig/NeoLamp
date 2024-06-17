// Rename for Update over the air
#define FILENAME_VERSION "NeoLamp_v1.0.6.bin"

// Change this names to your need
#define NAME "Nachtlicht"
#define URL "nachtlicht"
#define NEOPIXEL_PIN 4 // Wemos D1 mini: port D2
#define NEOPIXEL_COUNT 16

// Settings for Finns Special Lamp with Blink LEDs
#define SPECIAL_LAMP_WITH_DOUBLEBLINK false
#define LED_BLUE 0   // Wemos D1 mini: port D3
#define LED_YELLOW 2 // Wemos D1 mini: port D4
#define SETTING_SPECIAL_LAMP_WITH_DOUBLEBLINK_FS "/setting_lampdoubleblink.txt"

// Do not change this strings due to compatibility issues
#define SETTING_NAME_FS "/setting_lampname.txt"
#define SETTING_URL_FS "/setting_lampurl.txt"
#define SETTING_NEOPIXEL_PIN_FS "/setting_neopixel_pin.txt"
#define SETTING_NEOPIXEL_COUNT_FS "/setting_neopixel_count.txt"
