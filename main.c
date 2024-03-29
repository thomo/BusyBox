// #pragma config statements should precede project file includes.
#include "config.h"

#include <xc.h>                // Compiler library
#include <pic16f876a.h>        // Include library for PIC16F877A0

#include "macro.h"
#include "ledbar.h"
#include "ledring.h"
#include "ledblink.h"

//int tick_count;
//
//void __interrupt(high_priority) tcInt(void)
//{
//    if (TMR0IE && TMR0IF) {  // any timer 0 interrupts?
//        TMR0IF=0;
//        ++tick_count;
//    }
//    if (TMR1IE && TMR1IF) {  // any timer 1 interrupts?
//        TMR1IF=0;
//        tick_count += 100;
//    }
//    // process other interrupt sources here, if required
//    return;
//}
//
//void startAnalogRead(unsigned char input) {
//    ADCON0bits.CHS = input;
//    ADCON0bits.GO = 1;
//}
//
//unsigned char getAnalogValue() {
//    while(ADCON0bits.GO) {
//        /* wait */
//    }
//    
//    return ADRESH;
//}

void nextMode() {
    nextBarMode();
    nextRingMode();
    nextBlinkMode();
}

void prevMode() {
    prevBarMode();
    prevRingMode();
    prevBlinkMode();
}

// INPUT
// A0 analog
// A1 analog
// A4,A5 direction

// OUTPUT
// A2, A3 blink
// B0-B7  led bar
// C6,C7  led bar
// C1-C5  led circle

void updateOutputs() {
    unsigned char tempC;
    
    // A3, A2 blink
    PORTA = (unsigned char) (blinkValue << 2 & 0x06) ^ 0x06;
    
    // B7-B0  led bar
    // C7,C6  led bar
    PORTB = LSB(barValue) ^ 0xFF;
    tempC = (unsigned char) (MSB(barValue) << 6);

    // C5-C1  led circle
    tempC =  (unsigned char) (tempC | (ringValue  << 1));
    PORTC = tempC ^ 0xFF;
}

void init() {
    TRISA = 0b00110011;
    TRISB = 0x00;
    TRISC = 0b00000001;
    CMCON = 0x03;              // Comperator off, Analog input
    
    ADCON0bits.ADON  = 1;      // ADC on

    ADCON0bits.ADCS  = 0;      // Fosc/4
    ADCON1bits.ADCS2 = 1;
    
    ADCON1bits.ADFM  = 0;
    
    ADCON1bits.PCFG  = 0;      // analog input
}

void main() {
    init();
    
    unsigned char analogInput = 0;
    unsigned char av[2];
    
    av[0] = 0x00;
    av[1] = 0x00;
    
    while (TRUE){
        
//        startAnalogRead(analogInput);
//        av[analogInput] = getAnalogValue();
//        analogInput = (analogInput + 1) & 0x01;

        __delay_ms(100);      

        updateBarValue();
        updateRingValue();
        updateBlinkValue();
        
        updateOutputs();
    }
}