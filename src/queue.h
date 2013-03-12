/* 
 * File:   queue.h
 * Author: Nathaniel Abalos
 *
 * Created on March 7, 2013, 10:06 PM
 */

#ifndef QUEUE_H
#define	QUEUE_H


typedef struct QUEUE_S{
    UINT16 count;
    NODE * head;
    NODE * tail;
}QUEUE;

QUEUE * queue(void);
void * dequeue(QUEUE * q);
void enqueue(QUEUE * q, void * data);
UINT16 count(QUEUE * q);
void * head(QUEUE * q);

#endif	/* QUEUE_H */

