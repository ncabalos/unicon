#include <stdlib.h>
#include <GenericTypeDefs.h>
#include "datastructure.h"
#include "queue.h"

QUEUE * queue(void){
    QUEUE * q = malloc(sizeof(QUEUE));
    q->count = 0;
    q->head = NULL;
    q->tail = NULL;

    return q;
}

void enqueue(QUEUE * q, void * data){
    NODE * newnode;

    newnode = malloc(sizeof(NODE));

    newnode->data = data;

    if(q->count == 0){
        q->head = newnode;
        q->tail = newnode;
    }
    else{
        q->tail->next = newnode;
        newnode->prev = q->tail;
        q->tail = newnode;
    }
    q->count++;
}
void * dequeue(QUEUE * q){
    void * data;
    NODE * oldnode;
    if(q->count == 0) return NULL;

    oldnode = q->head;
    data = oldnode->data;

    if(q->count == 1){
        q->tail = NULL;
        q->head = NULL;
    }
    else{
        q->head = q->head->next;
        q->head->prev = NULL;
    }

    q->count--;

    free(oldnode);

    return data;
}

UINT16 count(QUEUE * q){
    return q->count;
}

void * head(QUEUE * q){
    return q->head->data;
}


