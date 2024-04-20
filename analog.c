#include <xc.h>
#include "analog.h"
#include "macro.h"

void nextAnalogChannel() {
    if (testbit(ADCON0, ADCON0bits.CHS0)) {
        clrbit(ADCON0, ADCON0bits.CHS0);
    } else {
        setbit(ADCON0, ADCON0bits.CHS0);
    }
}

void startAnalogConversion() {
    ADCON0bits.GO = 1;    // Starts A/D Conversion
}

void readAnalogChannel(){
    if (testbit(ADCON0, ADCON0bits.CHS0)) {
        poti[1] = ADRESH;
    } else {
        poti[0] = ADRESH;
    }
}
