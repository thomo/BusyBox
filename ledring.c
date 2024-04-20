#include "ledring.h"
#include "macro.h"
#include "random.h"

typedef enum {
    FUN_RING_RANDOM,
    FUN_RING_ROTATE_LEFT,
    FUN_RING_ROTATE_RIGHT,
    FUN_RING_BOUNCE_ON,
    FUN_RING_BOUNCE_OFF,
    FUN_RING_CENTER_OUT,
    FUN_RING_CENTER_IN,
    FUN_RING_BLINK,
    FUN_RING_COUNT_UP,
    FUN_RING_COUNT_DOWN
} RingFunId;

typedef struct { 
    RingFunId funId; 
    unsigned char init; 
} RingMode;

static const RingMode ringModes[] = { 
    //                         s  54321
    { FUN_RING_RANDOM,       0b00000001 },
    { FUN_RING_ROTATE_LEFT,  0b00000001 },
    { FUN_RING_ROTATE_LEFT,  0b00000011 },
    { FUN_RING_ROTATE_LEFT,  0b00000111 },
    { FUN_RING_ROTATE_LEFT,  0b00001111 },
    { FUN_RING_ROTATE_LEFT,  0b00011110 },
    { FUN_RING_ROTATE_RIGHT, 0b00011100 },
    { FUN_RING_ROTATE_RIGHT, 0b00011000 },
    { FUN_RING_ROTATE_RIGHT, 0b00010000 },
    { FUN_RING_BOUNCE_ON,    0b00010000 },
    { FUN_RING_BOUNCE_ON,    0b00011000 },
    { FUN_RING_BOUNCE_ON,    0b00011100 },
    { FUN_RING_BOUNCE_ON,    0b00011110 },
    { FUN_RING_BOUNCE_OFF,   0b10011110 },
    { FUN_RING_BOUNCE_OFF,   0b10011100 },
    { FUN_RING_BOUNCE_OFF,   0b10011000 },
    { FUN_RING_BOUNCE_OFF,   0b10010000 },
    { FUN_RING_CENTER_OUT,   0b00000100 },
    { FUN_RING_CENTER_OUT,   0b00001110 },
    { FUN_RING_CENTER_OUT,   0b00011111 },
    { FUN_RING_CENTER_IN ,   0b00001110 },
    { FUN_RING_CENTER_IN,    0b00000100 },
    { FUN_RING_BLINK    ,    0b00000000 },
    { FUN_RING_COUNT_UP ,    0b00000000 },
    { FUN_RING_COUNT_DOWN,   0b00011111 },
};

unsigned char numRingModes = sizeof(ringModes) / sizeof(ringModes[0]);

unsigned char ringMode = 0;
unsigned char ringTemp = 0;

void initNewRingMode() {
    ringValue = ringModes[ringMode].init & 0x1F;
    ringTemp = ringModes[ringMode].init & 0x80 >> 7;
}

void nextRingMode() {
    ringMode += 1;
    if (ringMode >= numRingModes) ringMode = 0;
    
    initNewRingMode();
}

void prevRingMode() {
    if (ringMode == 0) 
        ringMode = numRingModes; 
    ringMode -= 1;
    
    initNewRingMode();
}

void rotateRing(unsigned char dir) {
    if ((dir & 0x01) == 0) {
        ringValue =  (unsigned char) (ringValue << 1);
        if (testbit(ringValue, 5)) {
            ringValue = ringValue | 0x01;
        }
        ringValue = ringValue & 0x1F;
    } else {
        if (testbit(ringValue, 0)) {
            ringValue = ringValue >> 1;
            ringValue = ringValue | 0x10;
        } else {
            ringValue = ringValue >> 1;
        }
    }
}

void bounceRing(unsigned char mode) {
    if (mode == FUN_MODE_ON) {
        // move "1"
        if (ringTemp == FUN_MODE_RIGHT) {
            if (testbit(ringValue, 0)) {
                ringTemp = FUN_MODE_LEFT;
                ringValue = ((unsigned char)(ringValue << 1));
            } else {
                ringValue = ringValue >> 1;
            }
        } else {
            if (testbit(ringValue, 4)) {
                ringTemp = FUN_MODE_RIGHT;
                ringValue = ringValue >> 1;
            } else {
                ringValue = ((unsigned char)(ringValue << 1));
            }
        }
    } else {
        // move "0"
        if (ringTemp == FUN_MODE_RIGHT) {
            if (!testbit(ringValue, 0)) {
                ringTemp = FUN_MODE_LEFT;
                ringValue = ((unsigned char)(ringValue << 1)) | 0x01;
            } else {
                ringValue = ringValue >> 1 | 0x10;
            }
        } else {
            if (!testbit(ringValue, 4)) {
                ringTemp = FUN_MODE_RIGHT;
                ringValue = ringValue >> 1 | 0x10;
            } else {
                ringValue = ((unsigned char)(ringValue << 1)) | 0x01;
            }
        }
    }
}

void centerRing(unsigned char dir) {
    // 0003 2123
    //       ...
    // take maked bits and calculate next value
    // then copy state to upper bits
    ringTemp = ringValue;
    if (dir == FUN_MODE_OUT) {
        ringTemp &= 0x07;
        if (testbit(ringTemp,0)) setbit(ringTemp,3);
        ringTemp = ringTemp >> 1;
    } else {
        ringTemp = (unsigned char) (ringTemp << 1);
        if (testbit(ringTemp,3)) setbit(ringTemp,0);
        ringTemp &= 0x07;
    }
    ringValue = ringTemp;
    if (testbit(ringTemp,0)) setbit(ringValue,3);
    if (testbit(ringTemp,1)) setbit(ringValue,4);
}

void blinkRing() {
    ringValue ^= 0x1F;
}

void countRing(unsigned char funMode) {
    if (funMode == FUN_MODE_UP) {
        ++ringValue;
    } else {
        --ringValue;
    } 
}

void randomRing() {
    ringValue = nextRandom8();
}

void updateRingValue() {
    RingFunId funId = ringModes[ringMode].funId;
    switch(funId) {
        case FUN_RING_RANDOM: randomRing();break;        
        case FUN_RING_ROTATE_LEFT: rotateRing(FUN_MODE_LEFT);break;
        case FUN_RING_ROTATE_RIGHT: rotateRing(FUN_MODE_RIGHT);break;  
        case FUN_RING_BOUNCE_ON: bounceRing(FUN_MODE_ON);break;
        case FUN_RING_BOUNCE_OFF: bounceRing(FUN_MODE_OFF);break;
        case FUN_RING_CENTER_OUT: centerRing(FUN_MODE_OUT);break;
        case FUN_RING_CENTER_IN: centerRing(FUN_MODE_IN);break;
        case FUN_RING_BLINK: blinkRing();break;
        case FUN_RING_COUNT_UP: countRing(FUN_MODE_UP);break;
        case FUN_RING_COUNT_DOWN: countRing(FUN_MODE_DOWN);break;
    }
    
}
