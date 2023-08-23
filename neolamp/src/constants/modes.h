#define STATE_ANIMATION_MIX "mix"
#define STATE_ANIMATION_PULSE "pulse"
#define STATE_ANIMATION_CIRCLE "circle"
#define STATE_ANIMATION_RAINBOW "rainbow"
#define STATE_ANIMATION_PICK "pick"
#define STATE_ANIMATION_GREEN "green"
#define STATE_ANIMATION_RED "orange"
#define STATE_ANIMATION_OFF "off"

const char *array_of_modes[][2] = {
    {"Mixed", STATE_ANIMATION_MIX},
    {"Pulsieren", STATE_ANIMATION_PULSE},
    {"Farbkeise", STATE_ANIMATION_CIRCLE},
    {"Regenbogen", STATE_ANIMATION_RAINBOW},
    {"Farbauswahl", STATE_ANIMATION_PICK},
    {"Gruen", STATE_ANIMATION_GREEN},
    {"Orange", STATE_ANIMATION_RED},
    {"Lampe Ausgeschaltet", STATE_ANIMATION_OFF}};
