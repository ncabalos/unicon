#include <stdlib.h>
#include <GenericTypeDefs.h>
#include "datastructure.h"
#include "stack.h"

STACK * stack(void){
    STACK * s = malloc(sizeof(STACK));
    s->count = 0;
    s->top = NULL;

    return s;
}
void push(STACK * s, void * data){
    NODE * newnode;

    newnode = malloc(sizeof(NODE));
    newnode->data = data;
    newnode->next = NULL;
    newnode->prev = NULL;
    if(s->count == 0){
        s->top = newnode;
        s->count++;
    }
    else{
        newnode->prev = s->top;
        s->top->next = newnode;
        s->top = newnode;
        s->count++;
    }
}
void * pop(STACK * s){
    NODE * oldnode;
    void * data;

    if(s->count == 0) return NULL;

    oldnode = s->top;
    data = oldnode->data;

    if(s->count == 1){
        s->top = NULL;
        s->count--;
    }
    else{
        s->top = oldnode->prev;
        s->top->next = NULL;
        s->count--;
    }

    free(oldnode);
    return data;
}
void * top(STACK * s){
    return s->top->data;
}

UINT16 scount(STACK * s){
    return s->count;
}
