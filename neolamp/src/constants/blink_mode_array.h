#include "blink_modes.h"

const char *blink_modes[][2] = {
    {"Off", D_LED_MODE_OFF},
    {"Blink", D_LED_MODE_BLINK},
    {"Blaue LED an", D_LED_MODE_BLUE},
    {"Gelbe LED an", D_LED_MODE_YELLOW},
    {"Blaue LED blinken", D_LED_MODE_BLUE_BLINK},
    {"Gelbe LED blinken", D_LED_MODE_YELLOW_BLINK}};