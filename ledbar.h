#ifndef LEDBAR_H
#define	LEDBAR_H

unsigned int barValue = 0x0001;

void nextBarMode(void);
void prevBarMode(void);

void updateBarValue(void);

#endif	/* LEDBAR_H */

