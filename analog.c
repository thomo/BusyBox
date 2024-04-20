#include <xc.h>
#include "analog.h"
#include "macro.h"

void nextAnalogChannel() {
    if (testbit(ADCON0, _ADCON0_CHS0_POSITION)) {
        ADCON0bits.CHS0 = 0;
    } else {
        ADCON0bits.CHS0 = 1;
    }
}

void startAnalogConversion() {
    ADCON0bits.GO = 1;    // Starts A/D Conversion
}

void readAnalogChannel(){
    if (testbit(ADCON0, _ADCON0_CHS0_POSITION)) {
        poti[1] = ADRESH;
    } else {
        poti[0] = ADRESH;
    }
}
