#include "modes.h"

const char *modes[][2] = {{"Mixed", STATE_ANIMATION_MIX},
                          {"Pulsieren", STATE_ANIMATION_PULSE},
                          {"Farbkeise", STATE_ANIMATION_CIRCLE},
                          {"Regenbogen", STATE_ANIMATION_RAINBOW},
                          {"Farbauswahl", STATE_ANIMATION_PICK},
                          {"Gruen", STATE_ANIMATION_GREEN},
                          {"Orange", STATE_ANIMATION_RED},
                          {"Lampe Ausgeschaltet", STATE_ANIMATION_OFF}};

const int sizeof_modes = sizeof(modes) / sizeof(modes[0]);