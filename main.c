// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.

#include <xc.h>                // Compiler library
#include <pic16f876a.h>        // Include library for PIC16F877A0

#define _XTAL_FREQ 7372800     // Oscillator frequency (8 MHz)

#define TRUE 1 

#define LSB(a)  *((unsigned char *)&a)
#define MSB(a)  *(((unsigned char *)&a) + 1)
#define  testbit(var, bit)   ((var) & (1 <<(bit)))
#define  setbit(var, bit)    ((var) |= (1 << (bit)))
#define  clrbit(var, bit)    ((var) &= ~(1 << (bit)))

// INPUT
// A0 analog
// A1 analog
// A4,A5 direction

// OUTPUT
// A2, A3 blink
// B0-B7  led bar
// C6,C7  led bar
// C1-C5  led circle

typedef enum {
    FUN_BAR_ROTATE_LEFT,
    FUN_BAR_ROTATE_RIGHT,
    FUN_BAR_BOUNCE_ON,
    FUN_BAR_BOUNCE_OFF,
    FUN_BAR_CENTER_OUT,
    FUN_BAR_CENTER_IN,
    FUN_BAR_BLINK,
    FUN_BAR_COUNT_UP,
    FUN_BAR_COUNT_DOWN
} BarFunId;

typedef struct { 
    BarFunId funId; 
    unsigned int init; 
} BarMode;

static const BarMode barModes[] = { 
    //                              0987654321
    { FUN_BAR_ROTATE_LEFT,  0b0000000000000001 },
    { FUN_BAR_ROTATE_LEFT,  0b0000000000000011 },
    { FUN_BAR_ROTATE_LEFT,  0b0000000000000111 },
    { FUN_BAR_ROTATE_LEFT,  0b0000000000001111 },
    { FUN_BAR_ROTATE_LEFT,  0b0000000000011111 },
    { FUN_BAR_ROTATE_LEFT,  0b0000000000111111 },
    { FUN_BAR_ROTATE_LEFT,  0b0000000001111111 },
    { FUN_BAR_ROTATE_LEFT,  0b0000000011111111 },
    { FUN_BAR_ROTATE_LEFT,  0b0000000111111111 },
    { FUN_BAR_ROTATE_LEFT,  0b0000001111111111 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001111111110 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001111111100 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001111111000 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001111110000 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001111100000 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001111000000 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001110000000 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001100000000 },
    { FUN_BAR_ROTATE_RIGHT, 0b0000001000000000 },
    { FUN_BAR_BOUNCE_ON,    0b0000001000000000 },
    { FUN_BAR_BOUNCE_ON,    0b0000001100000000 },
    { FUN_BAR_BOUNCE_ON,    0b0000001110000000 },
    { FUN_BAR_BOUNCE_ON,    0b0000001111000000 },
    { FUN_BAR_BOUNCE_ON,    0b0000001111100000 },
    { FUN_BAR_BOUNCE_ON,    0b0000001111110000 },
    { FUN_BAR_BOUNCE_ON,    0b0000001111111000 },
    { FUN_BAR_BOUNCE_ON,    0b0000001111111100 },
    { FUN_BAR_BOUNCE_ON,    0b0000001111111110},
    { FUN_BAR_BOUNCE_ON,    0b0000001111111111 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001111111110 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001111111100 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001111111000 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001111110000 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001111100000 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001111000000 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001110000000 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001100000000 },
    { FUN_BAR_BOUNCE_OFF,   0b0000001000000000},
    { FUN_BAR_CENTER_OUT,   0b0000000000110000 },
    { FUN_BAR_CENTER_OUT,   0b0000000001111000 },
    { FUN_BAR_CENTER_OUT,   0b0000000011111100 },
    { FUN_BAR_CENTER_OUT,   0b0000000111111110 },
    { FUN_BAR_CENTER_OUT,   0b0000001111111111 },
    { FUN_BAR_CENTER_IN ,   0b0000000111111110 },
    { FUN_BAR_CENTER_IN,    0b0000000011111100 },
    { FUN_BAR_CENTER_IN,    0b0000000001111000 },
    { FUN_BAR_CENTER_IN,    0b0000000000110000 },
    { FUN_BAR_BLINK    ,    0b0000000000000000 },
    { FUN_BAR_COUNT_UP ,    0b0000000000000000 },
    { FUN_BAR_COUNT_DOWN,   0b0000001111111111 },
};

unsigned char numBarModes = sizeof(barModes) / sizeof(barModes[0]);
unsigned char barMode = 0;

unsigned char ringMode = 0;
unsigned char blinkMode = 0;

unsigned int ledBar = 0x0003;
unsigned char ledRing = 0x03;
unsigned char ledBlink = 0x01;

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

void rotateBar(unsigned char dir) {
    if ((dir & 0x01) == 0) {
        ledBar = ledBar << 1;
        if (testbit(ledBar, 10)) {
            ledBar = ledBar | 0x0001;
        }
        ledBar = ledBar & 0x03FF;
    } else {
        if (testbit(ledBar, 0)) {
            ledBar = ledBar >> 1;
            ledBar = ledBar | 0x0200;
        } else {
            ledBar = ledBar >> 1;
        }
    }
}

void rotateRing(unsigned char dir) {
    if ((dir & 0x01) == 0) {
        ledRing = ledRing << 1;
        if (testbit(ledRing, 5)) {
            ledRing = ledRing | 0x01;
        }
        ledRing = ledRing & 0x1F;
    } else {
        if (testbit(ledRing, 0)) {
            ledRing = ledRing >> 1;
            ledRing = ledRing | 0x10;
        } else {
            ledRing = ledRing >> 1;
        }
    }
}

void blinkSame() {
    // TODO
}

void blinkAlternating() {
    // TODO
}

void increaseMode() {
    barMode += 1;
    if (barMode == numBarModes) barMode = 0;

    ringMode += 1;
    if (ringMode == numRingModes) ringMode = 0;

    blinkMode += 1;
    if (blinkMode == numBlinkModes) blinkMode = 0;
}

void decreaseMode() {

}

void updateLedBar() {
    switch(barMode) {
        case 0: rotateBar(0); 
        case 1: rotateBar(1); 
    }
}

void updateLedRing() {
    switch(ringMode) {
        case 0: rotateRing(0); 
        case 1: rotateRing(1); 
    }
}

void updateBlink() {
    switch(ledBlinkMode) {
        case 0: blinkSame(); 
        case 1: blinkAlternating(); 
    }
}

void updateOutputs() {
    // A2, A3 blink
    // B7-B0  led bar
    PORTB = LSB(ledBar) ^ 0xFF;
    
    // C7,C6  led bar
    unsigned char tempC = MSB(ledBar)<<6;

    // C5-C1  led circle
    tempC = tempC | (ledRing << 1);
    PORTC = tempC ^ 0xFF;
}

void main() {
    unsigned char analogInput = 0;
    
    unsigned char av[2];
    
    av[0] = 0x00;
    av[1] = 0x00;
    
    TRISA = 0b00110011;
    TRISB = 0x00;
    TRISC = 0b00000001;
    CMCON = 0x03;              // Comperator off, Analog input
    
    ADCON0bits.ADON  = 1;      // ADC on

    ADCON0bits.ADCS  = 0;      // Fosc/4
    ADCON1bits.ADCS2 = 1;
    
    ADCON1bits.ADFM  = 0;
    
    ADCON1bits.PCFG  = 0;      // analog input
    
    while (TRUE){
        
//        startAnalogRead(analogInput);
//        av[analogInput] = getAnalogValue();
//        analogInput = (analogInput + 1) & 0x01;

        __delay_ms(500);      

        updateLedBar();
        updateLedRing();
        updateBlink();
        
        updateOutputs();
    }
}