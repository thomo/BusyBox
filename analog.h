#ifndef ANALOG_H
#define	ANALOG_H

volatile unsigned char poti[2];

void nextAnalogChannel(void);

// stores analog value in poti[]
void readAnalogChannel(void);

void startAnalogConversion(void);

#endif	/* ANALOG_H */

