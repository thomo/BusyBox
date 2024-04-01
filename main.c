// #pragma config statements should precede project file includes.
#include "config.h"

#include <xc.h>                // Compiler library
#include <pic16f876a.h>        // Include library for PIC16F877A0

#include "macro.h"
#include "ledbar.h"
#include "ledring.h"
#include "ledblink.h"
#include "rotary.h"

#define TMR0_PRESET 26

int tick_count;

void __interrupt() ir(void)
{
    PORTCbits.RC0 = 1;
    // Timer0 Interrupt - Freq = 1001.74 Hz - Period = 0.000998 seconds
    if (TMR0IE && TMR0IF) {
      TMR0IF = 0;    // clear the flag
      TMR0 = TMR0_PRESET;   // reset the timer preset count
      
      // A5,A4 rotary encoder
      // 00110000
      rotaryEncoderTick((unsigned char)(PORTA & 0b00110000) >> 4);
    }
    PORTCbits.RC0 = 0;
    return;
}
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
    PORTA = (unsigned char) (blinkValue << 2 & 0b00001100) ^ 0b00001100;
    
    // B7-B0  led bar
    // C7,C6  led bar
    PORTB = LSB(barValue) ^ 0xFF;
    tempC = (unsigned char) (MSB(barValue) << 6);

    // C5-C1  led circle
    tempC =  (unsigned char) (tempC | (ringValue  << 1));
    PORTC = tempC ^ 0xFF;
}

void init() {
    TRISA = 0b00110011;         // A5,A4 digital input 
                                // A3,A2 digital output
                                // A1,A0 analog input
    TRISB = 0x00;               // B7-B0 output
    TRISC = 0b00000000;         // C7-C1 output
                                // C0 output (debug)
    
    CMCON = 0x03;               // Comperator off, Analog input
    
    ADCON0bits.ADON  = 1;       // ADC on

    ADCON0bits.ADCS  = 0;       // Fosc/4
    ADCON1bits.ADCS2 = 1;
    
    ADCON1bits.ADFM  = 0;
    
    ADCON1bits.PCFG  = 0x06;    // analog input
    
    
    // Timer0 Registers Prescaler= 8 - TMR0 Preset = 26 - Freq = 1001.74 Hz - Period = 0.000998 seconds
    OPTION_REGbits.T0CS = 0;  // bit 5  TMR0 Clock Source Select bit...0 = Internal Clock (CLKO) 1 = Transition on T0CKI pin
    OPTION_REGbits.T0SE = 0;  // bit 4 TMR0 Source Edge Select bit 0 = low/high 1 = high/low
    OPTION_REGbits.PSA = 0;   // bit 3  Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
    
    OPTION_REGbits.PS2 = 0;   // bits 2-0  PS2:PS0: Prescaler Rate Select bits
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 0;
    TMR0 = TMR0_PRESET;       // preset for timer register

    // Interrupt Registers
    INTCON = 0;               // clear the interrpt control register
    INTCONbits.TMR0IE = 1;    // bit5 TMR0 Overflow Interrupt Enable bit...1 = Enables the TMR0 interrupt
    INTCONbits.TMR0IF = 0;    // bit2 clear timer 0 interrupt flag
    INTCONbits.GIE = 1;       // bit7 global interrupt enable
    
    rotaryMode = ROTARY_MODE_FOUR3;
}

void main() {
    init();
    
    unsigned char analogInput = 0;
    unsigned char av[2];
    
    av[0] = 0x00;
    av[1] = 0x00;
    
    unsigned char currentPos = rotaryEncoderPos;
    unsigned char prevRotEncPos = currentPos;
    
    while (TRUE){
        
//        startAnalogRead(analogInput);
//        av[analogInput] = getAnalogValue();
//        analogInput = (analogInput + 1) & 0x01;

        __delay_ms(100);      

        currentPos = rotaryEncoderPos;
        
        if (prevRotEncPos < currentPos) {
            nextMode();
            prevRotEncPos = currentPos;
        } else if (prevRotEncPos > currentPos) {
            prevMode();
            prevRotEncPos = currentPos;
        }
        
        updateBarValue();
        updateRingValue();
        updateBlinkValue();
        
        updateOutputs();
    }
}