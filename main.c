// #pragma config statements should precede project file includes.
#include "config.h"

#include <xc.h>                // Compiler library
#include <pic16f876a.h>        // Include library for PIC16F877A0

#include "macro.h"
#include "ledbar.h"
#include "ledring.h"
#include "ledblink.h"
#include "rotary.h"
#include "analog.h"

#define TMR0_PRESET 218

static const unsigned int delayPreset[256] = {
  10,  11,  12,  13,  14,  15,  16,  17,   18,  19,  20,  21,  22,  23,  24,  25,
  26,  27,  28,  29,  30,  31,  32,  33,   34,  35,  36,  37,  38,  39,  40,  41,
  42,  43,  44,  45,  46,  47,  48,  49,   50,  51,  52,  53,  54,  55,  56,  57,  
  58,  59,  60,  61,  62,  63,  64,  65,   66,  67,  68,  69,  70,  71,  72,  73,  
  74,  75,  76,  77,  78,  79,  80,  81,   82,  83,  84,  85,  86,  87,  89,  91,  
  93,  95,  97,  99, 101, 103, 105, 107,  109, 111, 113, 115, 117, 119, 121, 123, 
 125, 127, 129, 131, 133, 135, 137, 139,  141, 143, 145, 147, 149, 151, 153, 155, 
 157, 159, 161, 163, 165, 167, 169, 171,  173, 176, 179, 182, 185, 188, 191, 194, 
 197, 200, 203, 206, 209, 212, 215, 218,  221, 224, 227, 230, 233, 236, 239, 242, 
 245, 248, 251, 254, 257, 260, 264, 268,  272, 276, 280, 284, 288, 292, 296, 300, 
 304, 308, 312, 316, 320, 324, 328, 332,  336, 340, 344, 348, 353, 358, 363, 368, 
 373, 378, 383, 388, 393, 398, 403, 408,  413, 418, 423, 428, 433, 439, 445, 451, 
 457, 463, 469, 475, 481, 487, 493, 499,  505, 511, 517, 523, 530, 537, 544, 551, 
 558, 565, 572, 579, 586, 593, 600, 607,  615, 623, 631, 639, 647, 655, 663, 671, 
 679, 687, 695, 704, 713, 722, 731, 740,  749, 758, 767, 776, 786, 796, 806, 816, 
 826, 836, 846, 856, 866, 877, 888, 899,  910, 921, 932, 943, 954, 966, 978, 990};

int tick_count;

void __interrupt() ir(void)
{
    // Timer0 Interrupt - Freq = 6063.16 Hz - Period = 0.000165 seconds
    if (TMR0IE && TMR0IF) {
      TMR0IF = 0;    // clear the flag
      TMR0 = TMR0_PRESET;   // reset the timer preset count
      
      readAnalogChannel();
      nextAnalogChannel();
      
      // A5,A4 rotary encoder
      // 00110000
      rotaryEncoderTick((unsigned char)(PORTA & 0b00110000) >> 4);
      
      startAnalogConversion();
    }
    return;
}

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
    PORTB = LSB(barValue) ^ 0xFF;

    // C7,C6  led bar    = barValue.MSB.1 + barValue.MSB.0
    // C5-C1  led circle = ringValue.4..0
    tempC  = (unsigned char) (MSB(barValue) << 6);
    tempC |= (unsigned char) (ringValue  << 1);
    PORTC = tempC ^ 0xFF;
}

void init() {
    PORTA = 0xFF;
    PORTB = 0xFF;
    PORTC = 0xFF;
    
    TRISA = 0b00110011;         // A5,A4 digital input 
                                // A3,A2 digital output
                                // A1,A0 analog input
    TRISB = 0x00;               // B7-B0 output
    TRISC = 0b00000000;         // C7-C1 output
                                // C0 output (debug)
    
    CMCON = 0x03;               // Comperator off, Analog input
    
    ADCON0bits.ADON  = 1;       // ADC on

    ADCON0bits.ADCS  = 2;       // Fosc/64
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
    
    rotaryMode = ROTARY_MODE_FOUR3;
    initRotaryPos();
    
    barValue = 0x0001;
    ringValue = 0x01;

    INTCONbits.GIE = 1;       // bit7 global interrupt enable
}

void main() {
    init();
    
    unsigned char currentPos = rotaryEncoderPos;
    unsigned char prevRotEncPos = currentPos;
    
    unsigned int delayLoop = 0;
    unsigned int delayEnd = 0;
    
    while (TRUE){

        __delay_ms(1);      

        delayEnd = delayPreset[poti[0]]; 
        
        currentPos = rotaryEncoderPos;

        if (prevRotEncPos < currentPos) {
            nextMode();
            prevRotEncPos = currentPos;
        } else if (prevRotEncPos > currentPos) {
            prevMode();
            prevRotEncPos = currentPos;
        }
        
        if (currentPos <= rotaryEncoderPosLimits[0] || currentPos >= rotaryEncoderPosLimits[1]) {
            INTCONbits.GIE = 0;       // global interrupt disable
            initRotaryPos();
            currentPos = rotaryEncoderPos;
            prevRotEncPos = currentPos;
            INTCONbits.GIE = 1;       // global interrupt enable
        } 

        updateOutputs();

        delayLoop = delayLoop + 1;
        
        if (delayLoop >= delayEnd) {
            delayLoop = 0;
            updateBarValue();
            updateRingValue();
            updateBlinkValue();
        }
    }
}