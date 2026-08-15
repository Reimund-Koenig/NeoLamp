#include "modes.h"

const char *modes[][2] = {
    {"Konstant Rot", MODE_COLOR_RED},
    {"Konstant Orange", MODE_COLOR_ORANGE},
    {"Konstant Gelb", MODE_COLOR_YELLOW},
    {"Konstant Hellgruen", MODE_COLOR_LIGHT_GREEN},
    {"Konstant Gruen", MODE_COLOR_GREEN},
    {"Konstant Turkis", MODE_COLOR_TURQUOISE},
    {"Konstant Blau", MODE_COLOR_BLUE},
    {"Konstant Lila", MODE_COLOR_PURPLE},
    {"Konstant Magenta", MODE_COLOR_MAGENTA},
    {"Konstant Weiss", MODE_COLOR_WHITE},
    {"Mode gemischt", MODE_COLOR_MIX},
    {"Mode Farbkreis im Uhrzeigersinn", MODE_COLOR_CIRCLE},
    {"Mode Farbkreis fuellend", MODE_COLOR_CIRCLE_FILLED},
    {"Mode Regenbogen", MODE_COLOR_RAINBOW}};

const int sizeof_modes = sizeof(modes) / sizeof(modes[0]);
