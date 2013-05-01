/* 
 * File:   stack.h
 * Author: Nathaniel Abalos
 *
 * Created on March 11, 2013, 7:10 PM
 */

#ifndef STACK_H
#define	STACK_H

typedef struct STACK_S{
    UINT16 count;
    NODE * top;
}STACK;

STACK * stack(void);
void push(STACK * s, void * data);
void * pop(STACK * s);
void * top(STACK * s);
UINT16 scount(STACK * s);

#endif	/* STACK_H */

