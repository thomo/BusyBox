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
#define PWMLOOP_PRESET 150

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

static const unsigned char pwmCycle[256] = {
 68,  71,  74,  77,  79,  82,  85,  89,  92,  95,  98, 101, 105, 108, 112, 115,
119, 122, 126, 130, 134, 137, 141, 145, 149, 153, 157, 160, 164, 168, 172, 176, 
180, 184, 187, 191, 195, 198, 202, 206, 209, 212, 216, 219, 222, 225, 228, 230, 
233, 236, 238, 240, 242, 244, 246, 248, 249, 250, 252, 252, 253, 254, 254, 255, 
255, 255, 254, 254, 253, 253, 252, 250, 249, 248, 246, 244, 242, 240, 238, 236, 
233, 230, 228, 225, 222, 219, 216, 212, 209, 206, 202, 199, 195, 191, 187, 184, 
180, 176, 172, 168, 164, 161, 157, 153, 149, 145, 141, 137, 134, 130, 126, 123, 
119, 115, 112, 108, 105, 102,  98,  95,  92,  89,  86,  82,  80,  77,  74,  71,  
 68,  66,  63,  61,  58,  56,  54,  51,  49,  47,  45,  43,  41,  39,  37,  36,  
 34,  32,  31,  29,  28,  26,  25,  23,  22,  21,  20,  18,  17,  16,  15,  14,  
 13,  12,  12,  11,  10,   9,   8,   8,   7,   6,   6,   5,   5,   4,   4,   3,   
  3,   2,   2,   2,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   2,   2,   2,   
  3,   3,   4,   4,   5,   5,   6,   6,   7,   8,   8,   9,  10,  11,  11,  12,  
 13,  14,  15,  16,  17,  18,  20,  21,  22,  23,  25,  26,  27,  29,  30,  32,  
 34,  35,  37,  39,  41,  43,  45,  47,  49,  51,  53,  56,  58,  60,  63,  66};

int tick_count;
volatile unsigned char pwmLoop;
volatile unsigned char pwmStep;

void __interrupt() ir(void)
{
    // http://eng-serve.com/pic/pic_timer.html
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
      
      if (pwmLoop == 0) {
        pwmStep = pwmStep + 1;
        CCPR1L = pwmCycle[pwmStep];
        pwmLoop = PWMLOOP_PRESET;
      } else {
        pwmLoop = pwmLoop - 1;
      }
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
    tempC  = (unsigned char) (MSB(barValue) << 6);
    // C5-C3  led circle = ringValue.4..2
    tempC |= ((unsigned char) (ringValue  << 1) & 0b00111000);
    // C1-C0  led circle = ringValue.1..0
    tempC |= (ringValue & 0b00000011);
    
    PORTC = tempC ^ 0xFF;
}

void init() {
    PORTA = 0xFF;
    PORTB = 0xFF;
    PORTC = 0xFF;
    
    TRISA = 0b00110011;         // A5,A4 digital input 
                                // A3,A2 digital output
                                // A1 unused (input)
                                // A0 analog input
    TRISB = 0x00;               // B7-B0 output
    TRISC = 0b00000000;         // C7-C0 output
    
    CMCON = 0x03;               // Comperator off, Analog input
    
    ADCON0bits.ADON  = 1;       // ADC on

    ADCON0bits.ADCS  = 2;       // Fosc/64
    ADCON1bits.ADCS2 = 1;
    
    ADCON1bits.ADFM  = 0;
    
    ADCON1bits.PCFG  = 0x06;    // analog input
    
    // PWM Init
    // https://www.micro-examples.com/public/microex-navig/doc/097-pwm-calculator.html
    // 1. Set the PWM period by writing to the PR2 register.
    PR2 = 0xFF;
    pwmStep = 0;
    pwmLoop = PWMLOOP_PRESET;
    // 2. Set the PWM duty cycle by writing to the CCPR1L register and CCP1CON<5:4> bits.
    CCPR1L = pwmCycle[0]; 
    CCP1CONbits.CCP1X = 0;
    CCP1CONbits.CCP1Y = 1;
    // 3. Make the CCP1 pin an output by clearing the TRISC<2> bit.
    // see above
    // 4. Set the TMR2 prescale value and enable Timer2 by writing to T2CON.
    T2CONbits.T2CKPS = 0; // Prescaler = 1 (PWM freq. 19.53 kHz)
    T2CONbits.TMR2ON = 1; // TMR2 enabled   
    // 5. Configure the CCP1 module for PWM operation.
    CCP1CONbits.CCP1M = 0x0F; // PWM Mode
    
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

        delayEnd = delayPreset[poti[1]]; 
        
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