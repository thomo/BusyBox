/* 
 * File:   macro.h
 * Author: Gisela
 *
 * Created on 29. März 2024, 12:40
 */

#ifndef MACRO_H
#define	MACRO_H

#define LSB(a)  *((unsigned char *)&a)
#define MSB(a)  *(((unsigned char *)&a) + 1)
#define testbit(var, bit)   ((var) & (1 <<(bit)))
#define setbit(var, bit)    ((var) |= (1 << (bit)))
#define clrbit(var, bit)    ((var) &= ~(1 << (bit)))

#define TRUE 1 

#endif	/* MACRO_H */

