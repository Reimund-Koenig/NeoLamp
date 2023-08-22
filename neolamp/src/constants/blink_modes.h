
#define D_LED_MODE_OFF "off"
#define D_LED_MODE_BLINK "blink"
#define D_LED_MODE_BLUE "blue"
#define D_LED_MODE_YELLOW "yellow"
#define D_LED_MODE_YELLOW_BLINK "yb"
#define D_LED_MODE_BLUE_BLINK "bb"

const char *array_of_blink_modes[][2] = {
    {"Off", D_LED_MODE_OFF},
    {"Blink", D_LED_MODE_BLINK},
    {"Blaue LED an", D_LED_MODE_BLUE},
    {"Gelbe LED an", D_LED_MODE_YELLOW},
    {"Blaue LED blinken", D_LED_MODE_BLUE_BLINK},
    {"Gelbe LED blinken", D_LED_MODE_YELLOW_BLINK}};
