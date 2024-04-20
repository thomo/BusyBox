#include "ledblink.h"
#include "macro.h"
#include "random.h"

typedef enum {
    FUN_BLINK_RANDOM,
    FUN_BLINK
} BlinkFunId;

typedef struct { 
    BlinkFunId funId; 
    unsigned char init; 
} BlinkMode;

static const BlinkMode blinkModes[] = { 
    { FUN_BLINK_RANDOM,  0b00000000 }, // same
    { FUN_BLINK,  0b00000000 }, // same
    { FUN_BLINK,  0b00000001 }  // alternate
};

unsigned char numBlinkModes = sizeof(blinkModes) / sizeof(blinkModes[0]);

unsigned char blinkMode = 0;

void initNewBlinkMode() {
    blinkValue = blinkModes[blinkMode].init;
}

void nextBlinkMode() {
    blinkMode += 1;
    if (blinkMode >= numBlinkModes) blinkMode = 0;
    
    initNewBlinkMode();
}

void prevBlinkMode() {
    if (blinkMode == 0) 
        blinkMode = numBlinkModes; 
    
    blinkMode -= 1;

    initNewBlinkMode();
}

void blink() {
    blinkValue ^= 0xFF;
}

void randomBlink() {
    blinkValue = nextRandom8();
}

void updateBlinkValue() {
    BlinkFunId funId = blinkModes[blinkMode].funId;
    switch(funId) {
        case FUN_BLINK_RANDOM: randomBlink(); break;
        case FUN_BLINK: blink(); break;
    }
}
