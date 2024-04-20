#ifndef MACRO_H
#define	MACRO_H

#define LSB(a)  *((unsigned char *)&a)
#define MSB(a)  *(((unsigned char *)&a) + 1)
#define testbit(var, bit)   ((var) &  (1 << (bit)))
#define setbit(var, bit)    ((var) |= (1 << (bit)))
#define clrbit(var, bit)    ((var) &= ~(1 << (bit)))

#define TRUE 1 

#define FUN_MODE_ON 1
#define FUN_MODE_OFF 0
#define FUN_MODE_LEFT 0
#define FUN_MODE_RIGHT 1
#define FUN_MODE_IN 0
#define FUN_MODE_OUT 1
#define FUN_MODE_UP 1
#define FUN_MODE_DOWN 0

#endif	/* MACRO_H */

