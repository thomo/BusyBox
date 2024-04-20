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
    //                        s     0987654321
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
    { FUN_BAR_BOUNCE_ON,    0b0000001111111110 },
    { FUN_BAR_BOUNCE_ON,    0b0000001111111111 },
    { FUN_BAR_BOUNCE_OFF,   0b1000001111111110 },
    { FUN_BAR_BOUNCE_OFF,   0b1000001111111100 },
    { FUN_BAR_BOUNCE_OFF,   0b1000001111111000 },
    { FUN_BAR_BOUNCE_OFF,   0b1000001111110000 },
    { FUN_BAR_BOUNCE_OFF,   0b1000001111100000 },
    { FUN_BAR_BOUNCE_OFF,   0b1000001111000000 },
    { FUN_BAR_BOUNCE_OFF,   0b1000001110000000 },
    { FUN_BAR_BOUNCE_OFF,   0b1000001100000000 },
    { FUN_BAR_BOUNCE_OFF,   0b1000001000000000},
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
unsigned char barTemp = 0;

void initNewBarMode() {    
    barValue = barModes[barMode].init & 0x03FF;
    barTemp = MSB(barModes[barMode].init) & 0x80 >> 7;
}

void nextBarMode() {
    barMode += 1;
    if (barMode >= numBarModes) barMode = 0;
    initNewBarMode();
}

void prevBarMode() {
    if (barMode == 0) 
        barMode = numBarModes; 

    barMode -= 1;
   
    initNewBarMode();
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
            barValue = barValue | 0x0400;
        } 
        barValue = barValue >> 1;
    }
}

void bounceBar(unsigned char mode) {
    if (mode == FUN_MODE_ON) {
        // move "1"
        if (barTemp == FUN_MODE_RIGHT) {
            if (testbit(barValue, 0)) {
                barTemp = FUN_MODE_LEFT;
                barValue = barValue << 1;
            } else {
                barValue = barValue >> 1;
            }
        } else {
            if (testbit(barValue, 9)) {
                barTemp = FUN_MODE_RIGHT;
                barValue = barValue >> 1;
            } else {
                barValue = barValue << 1;
            }
        }
    } else {
        // move "0"
        if (barTemp == FUN_MODE_RIGHT) {
            if (!testbit(barValue, 0)) {
                barTemp = FUN_MODE_LEFT;
                barValue = barValue << 1 | 0x0001;
            } else {
                barValue = barValue >> 1 | 0x0200;
            }
        } else {
            if (!testbit(barValue, 9)) {
                barTemp = FUN_MODE_RIGHT;
                barValue = barValue >> 1 | 0x0200;
            } else {
                barValue = barValue << 1 | 0x0001;
            }
        }
    }
} 

void centerBar(unsigned char dir) {
    if (dir == FUN_MODE_OUT) {
        barTemp = ((unsigned char)(MSB(barValue) << 3)) | ((LSB(barValue) & 0xE0) >> 5);
        barTemp = (unsigned char) (barTemp << 1);
        if (testbit(barTemp,5)) barTemp |= 0x01;
            
    }
}

void blinkBar(){
    barValue ^= 0xFFFF;
}

void countBar(unsigned char funMode) {
    if (funMode == FUN_MODE_UP) {
        ++barValue;
    } else {
        --barValue;
    } 
}

void updateBarValue() {
    BarFunId funId = barModes[barMode].funId;
    switch(funId) {
        case FUN_BAR_ROTATE_LEFT: rotateBar(FUN_MODE_LEFT);break;
        case FUN_BAR_ROTATE_RIGHT: rotateBar(FUN_MODE_RIGHT);break;  
        case FUN_BAR_BOUNCE_ON: bounceBar(FUN_MODE_ON);break;
        case FUN_BAR_BOUNCE_OFF: bounceBar(FUN_MODE_OFF);break;
        case FUN_BAR_CENTER_OUT: centerBar(FUN_MODE_OUT);break;
        case FUN_BAR_CENTER_IN: centerBar(FUN_MODE_IN);break;
        case FUN_BAR_BLINK: blinkBar();break;
        case FUN_BAR_COUNT_UP: countBar(FUN_MODE_UP);break;
        case FUN_BAR_COUNT_DOWN: countBar(FUN_MODE_DOWN);break;
    }
}

