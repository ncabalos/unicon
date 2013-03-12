/* 
 * File:   datastructure.h
 * Author: Nathaniel Abalos
 *
 * Created on March 11, 2013, 7:08 PM
 */

#ifndef DATASTRUCTURE_H
#define	DATASTRUCTURE_H

typedef struct NODE_S{
    struct NODE_S * prev;
    struct NODE_S * next;
    void * data;
}NODE;


#endif	/* DATASTRUCTURE_H */

