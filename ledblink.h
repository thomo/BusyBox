#ifndef LEDBLINK_H
#define	LEDBLINK_H

unsigned char blinkValue = 0x00;

void nextBlinkMode(void);
void prevBlinkMode(void);

void updateBlinkValue(void);

#endif	/* LEDBLINK_H */

