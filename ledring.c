#include "ledring.h"
#include "macro.h"

unsigned char ringMode = 0;

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

void updateRingValue() {
    
}

//void updateLedRing() {
//    switch(ringMode) {
//        case 0: rotateRing(0); 
//        case 1: rotateRing(1); 
//    }
//}
//
//void updateBlink() {
//    switch(ledBlinkMode) {
//        case 0: blinkSame(); 
//        case 1: blinkAlternating(); 
//    }
//}