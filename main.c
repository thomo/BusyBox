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

# define TRUE 1 

// OUTPUT
// PORTA (2,3)
// PORTB
// PORTC (but not RC0)

void main(){
    TRISA = 0b00110011;
    TRISB = 0x00;
    TRISC = 0b00000001;
    
    while (TRUE){
        PORTA = 0x00;          // all off
        PORTB = 0x00;          // all off
        PORTC = 0x00;          // all off
        __delay_ms(1000);      // Wait 1 sec.
        PORTA = 0xff;          // all on
        PORTB = 0xff;          // all on
        PORTC = 0xff;          // all on
        __delay_ms(1000);      // Wait 1 sec.          
    }
}