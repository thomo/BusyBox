#include "ledring.h"
#include "macro.h"

typedef enum {
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
    //                         87654321
    { FUN_RING_ROTATE_LEFT,  0b00000001 },
    { FUN_RING_ROTATE_LEFT,  0b00000011 },
    { FUN_RING_ROTATE_LEFT,  0b00000111 },
    { FUN_RING_ROTATE_LEFT,  0b00001111 },
    { FUN_RING_ROTATE_LEFT,  0b00011111 },
    { FUN_RING_ROTATE_LEFT,  0b00011110 },
    { FUN_RING_ROTATE_RIGHT, 0b00011100 },
    { FUN_RING_ROTATE_RIGHT, 0b00011000 },
    { FUN_RING_ROTATE_RIGHT, 0b00010000 },
    { FUN_RING_BOUNCE_ON,    0b00010000 },
    { FUN_RING_BOUNCE_ON,    0b00011000 },
    { FUN_RING_BOUNCE_ON,    0b00011100 },
    { FUN_RING_BOUNCE_ON,    0b00011110 },
    { FUN_RING_BOUNCE_ON,    0b00011111 },
    { FUN_RING_BOUNCE_OFF,   0b00011110 },
    { FUN_RING_BOUNCE_OFF,   0b00011100 },
    { FUN_RING_BOUNCE_OFF,   0b00011000 },
    { FUN_RING_BOUNCE_OFF,   0b00010000 },
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

void nextRingMode() {
    ++ringMode;
    if (ringMode == numRingModes) ringMode = 0;
    
    ringValue = ringModes[ringMode].init;
}

void prevRingMode() {
    if (ringMode == 0) 
        ringMode = numRingModes; 
    else 
        --ringMode;
    
    ringValue = ringModes[ringMode].init;
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

void bounceRing(unsigned char mode){}
void centerRing(unsigned char mode){}
void blinkRing(){}
void countRing(unsigned char mode){}

void updateRingValue() {
    RingFunId funId = ringModes[ringMode].funId;
    switch(funId) {
        case FUN_RING_ROTATE_LEFT: rotateRing(FUN_MODE_LEFT);break;
        case FUN_RING_ROTATE_RIGHT: rotateRing(FUN_MODE_RIGHT);break;  
        case FUN_RING_BOUNCE_ON: bounceRing(FUN_MODE_ON);break;
        case FUN_RING_BOUNCE_OFF: bounceRing(FUN_MODE_ON);break;
        case FUN_RING_CENTER_OUT: centerRing(FUN_MODE_ON);break;
        case FUN_RING_CENTER_IN: centerRing(FUN_MODE_ON);break;
        case FUN_RING_BLINK: blinkRing();break;
        case FUN_RING_COUNT_UP: countRing(FUN_MODE_ON);break;
        case FUN_RING_COUNT_DOWN: countRing(FUN_MODE_ON);break;
    }
    
}
