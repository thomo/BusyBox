#include "ledblink.h"
#include "macro.h"

typedef enum {
    FUN_BLINK
} BlinkFunId;

typedef struct { 
    BlinkFunId funId; 
    unsigned char init; 
} BlinkMode;

static const BlinkMode blinkModes[] = { 
                                 
    { FUN_BLINK,  0b00000000 }, // same
    { FUN_BLINK,  0b00000001 }  // alternate
};

unsigned char numBlinkModes = sizeof(blinkModes) / sizeof(blinkModes[0]);

unsigned char blinkMode = 0;

void nextBlinkMode() {
    ++blinkMode;
    if (blinkMode == numBlinkModes) blinkMode = 0;
    
    blinkValue = blinkModes[blinkMode].init;
}

void prevBlinkMode() {
    if (blinkMode == 0) 
        blinkMode = numBlinkModes; 
    else 
        --blinkMode;

    blinkValue = blinkModes[blinkMode].init;
}

void blink() {
    blinkValue ^= 0xFF;
}

void updateBlinkValue() {
    BlinkFunId funId = blinkModes[blinkMode].funId;
    switch(funId) {
        case FUN_BLINK: blink(); break;
    }
}
