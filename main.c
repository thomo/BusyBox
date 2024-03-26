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

void blink() {

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

        rotateBar(1);
        rotateRing(1);
        blink();

        updateOutputs();
    }
}