#include <GenericTypeDefs.h>
#include "task.h"

TASK tasklist[TASKLIST_MAX_SIZE];

void taskSchedule(TASK_ID task, TASK_EVENT event, TASK_DATA data){
    TASK *t;
    TASK_Q *eventq;
    TASK_Q *dataq;

    t = &tasklist[task];
    
    if(t->state == TASK_WAITING)
    {
    eventq = &(t->eventQ);
    dataq = &(t->dataQ);

    taskEnqueue(eventq, (TASK_Q_ELEMENT) event);
    taskEnqueue(dataq, (TASK_Q_ELEMENT) data);

    t->state = TASK_READY;
    }

}

void taskRegister(TASK_ID task, TASK_FUNCTION function){
    TASK *t;
    t = &tasklist[task];
    t->function = function;
    t->state = TASK_WAITING;
    t->dataQ.queueCount = 0;
    t->dataQ.queueIndexIn = 0;
    t->dataQ.queueIndexOut = 0;

    t->eventQ.queueCount = 0;
    t->eventQ.queueIndexIn = 0;
    t->eventQ.queueIndexOut = 0;
}

void taskProcess(){
    TASK *t;
    TASK_EVENT event;
    TASK_DATA data;
    UINT16 i;
    for(i = 0; i < TASKLIST_MAX_SIZE; i++){
        t = &tasklist[i];
        if(t->state == 0xAA)
            while(1);
        if(t->state == TASK_READY){
            t->state = TASK_RUNNING;
            event = (TASK_EVENT)taskDequeue(&(t->eventQ));
            data = (TASK_DATA)taskDequeue(&(t->dataQ));

            t->function(event, data);

            // function rescheduled itself
            if(t->state != TASK_READY)
                t->state = TASK_WAITING;

            return;
        }
    }
}

void taskEnqueue(TASK_Q *queue, TASK_Q_ELEMENT element)
{
    if(queue->queueCount < TASKQUEUE_MAX_SIZE){
        queue->data[queue->queueIndexIn] = element;
        queue->queueIndexIn++;
        if(queue->queueIndexIn > TASKQUEUE_MAX_SIZE) queue->queueIndexIn = 0;
        queue->queueCount++;
    }
}
TASK_Q_ELEMENT taskDequeue(TASK_Q *queue){

    TASK_Q_ELEMENT element;

    if(queue->queueCount > 0){
        element = queue->data[queue->queueIndexOut];
        queue->queueIndexOut++;
        if(queue->queueIndexOut > TASKQUEUE_MAX_SIZE) queue->queueIndexOut = 0;
        queue->queueCount--;
        return element;
    }
    return NULL;
}
