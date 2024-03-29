#include "ledbar.h"
#include "macro.h"

typedef enum {
    FUN_BAR_ROTATE_LEFT,
    FUN_BAR_ROTATE_RIGHT,
    FUN_BAR_BOUNCE_ON,
    FUN_BAR_BOUNCE_OFF,
    FUN_BAR_CENTER_OUT,
    FUN_BAR_CENTER_IN,
    FUN_BAR_BLINK,
    FUN_BAR_COUNT_UP,
    FUN_BAR_COUNT_DOWN
} BarFunId;

typedef struct { 
    BarFunId funId; 
    unsigned int init; 
} BarMode;

static const BarMode barModes[] = { 
    //                              0987654321
    { FUN_BAR_ROTATE_LEFT,  0b0000000000000001 },
    { FUN_BAR_ROTATE_LEFT,  0b0000000000000011 },
    { FUN_BAR_ROTATE_LEFT,  0b0000000000000111 },
    { FUN_BAR_ROTATE_LEFT,  0b0000000000001111 },
    { FUN_BAR_ROTATE_LEFT,  0b0000000000011111 },
    { FUN_BAR_ROTATE_LEFT,  0b0000000000111111 },
    { FUN_BAR_ROTATE_LEFT,  0b0000000001111111 },
    { FUN_BAR_ROTATE_LEFT,  0b0000000011111111 },
    { FUN_BAR_ROTATE_LEFT,  0b0000000111111111 },
    { FUN_BAR_ROTATE_LEFT,  0b0000001111111111 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001111111110 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001111111100 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001111111000 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001111110000 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001111100000 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001111000000 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001110000000 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001100000000 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001000000000 },
    { FUN_BAR_BOUNCE_ON,    0b0000001000000000 },
    { FUN_BAR_BOUNCE_ON,    0b0000001100000000 },
    { FUN_BAR_BOUNCE_ON,    0b0000001110000000 },
    { FUN_BAR_BOUNCE_ON,    0b0000001111000000 },
    { FUN_BAR_BOUNCE_ON,    0b0000001111100000 },
    { FUN_BAR_BOUNCE_ON,    0b0000001111110000 },
    { FUN_BAR_BOUNCE_ON,    0b0000001111111000 },
    { FUN_BAR_BOUNCE_ON,    0b0000001111111100 },
    { FUN_BAR_BOUNCE_ON,    0b0000001111111110},
    { FUN_BAR_BOUNCE_ON,    0b0000001111111111 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001111111110 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001111111100 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001111111000 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001111110000 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001111100000 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001111000000 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001110000000 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001100000000 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001000000000},
    { FUN_BAR_CENTER_OUT,   0b0000000000110000 },
    { FUN_BAR_CENTER_OUT,   0b0000000001111000 },
    { FUN_BAR_CENTER_OUT,   0b0000000011111100 },
    { FUN_BAR_CENTER_OUT,   0b0000000111111110 },
    { FUN_BAR_CENTER_OUT,   0b0000001111111111 },
    { FUN_BAR_CENTER_IN ,   0b0000000111111110 },
    { FUN_BAR_CENTER_IN,    0b0000000011111100 },
    { FUN_BAR_CENTER_IN,    0b0000000001111000 },
    { FUN_BAR_CENTER_IN,    0b0000000000110000 },
    { FUN_BAR_BLINK    ,    0b0000000000000000 },
    { FUN_BAR_COUNT_UP ,    0b0000000000000000 },
    { FUN_BAR_COUNT_DOWN,   0b0000001111111111 },
};

unsigned char numBarModes = sizeof(barModes) / sizeof(barModes[0]);

unsigned char barMode = 0;

void nextBarMode() {
    barMode += 1;
    if (barMode == numBarModes) barMode = 0;
}

void prevBarMode() {
    if (barMode == 0) 
        barMode = numBarModes; 
    else 
        barMode -= barMode;
}

void rotateBar(unsigned char dir) {
    if ((dir & 0x01) == 0) {
        barValue = barValue << 1;
        if (testbit(barValue, 10)) {
            barValue = barValue | 0x0001;
        }
        barValue = barValue & 0x03FF;
    } else {
        if (testbit(barValue, 0)) {
            barValue = barValue >> 1;
            barValue = barValue | 0x0200;
        } else {
            barValue = barValue >> 1;
        }
    }
}

void updateBarValue() {
    switch(barMode) {
        case 0: rotateBar(0); 
        case 1: rotateBar(1); 
    }
}

