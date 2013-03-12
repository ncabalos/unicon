#include <GenericTypeDefs.h>
#include "task.h"

///Added for critical section
#define CRITICAL_SECTION_START asm volatile ("disi #0x3FFF");
#define CRITICAL_SECTION_END asm volatile ("disi #0");

TASK_QUEUE taskQueues[TASK_PRIORITY_MAX];
TASK_DELAY_ELEMENT taskDelayList[TASKQUEUE_MAX_SIZE];

BOOL taskSubmit(TASK_PRIORITY priority, TASK task, EVENT event, void *data, UINT16 delay) {
    TASK_QUEUE *queue;
    TASK_ELEMENT *element;
    TASK_DELAY_ELEMENT *delayElement;
    UINT16 i;

    if (priority != TASK_PRIORITY_TIMED) {

        queue = &taskQueues[priority];

        if (queue->count < TASKQUEUE_MAX_SIZE) {
            element = &queue->queue[queue->in];
            element->task = task;
            element->data = data;
            element->event = event;

            queue->in++;
            if (queue->in == TASKQUEUE_MAX_SIZE) queue->in = 0;
            queue->count++;
            return TRUE;
        } else
            return FALSE;
    } else {
        // add to delayed task list
        for (i = 0; i < TASKQUEUE_MAX_SIZE; i++) {
            delayElement = &taskDelayList[i];
            if (delayElement->isValid == FALSE) {
                delayElement->isValid = TRUE;
                delayElement->counter = delay;
                element = &delayElement->element;
                element->task = task;
                element->data = data;
                element->event = event;
                return TRUE;
            }
        }
    }
    return FALSE;
}

void taskProcess(void) {
    UINT16 queueIndex;
    TASK_QUEUE *queue;
    TASK_ELEMENT *element;
    TASK_DELAY_ELEMENT *delayElement;
    UINT16 index;

    for (queueIndex = 0; queueIndex < TASK_PRIORITY_MAX; queueIndex++) {
        queue = &taskQueues[queueIndex];
        if (queue->count) {
            element = &queue->queue[queue->out];
            queue->out++;
            if (queue->out == TASKQUEUE_MAX_SIZE) queue->out = 0;
            queue->count--;
            element->task(element->event, element->data);
            break;
        }
    }

    //process delay task list
    for(index = 0; index < TASKQUEUE_MAX_SIZE; index++){
        delayElement = &taskDelayList[index];
        if(delayElement->isValid){
            if(delayElement->counter == 0){
                element = &delayElement->element;
                TASK_SUBMIT_NORMAL(element->task, element->event, element->data);
                delayElement->isValid = FALSE;
            }
        }
    }
}

void taskProcessDelayList(void)
{
    TASK_DELAY_ELEMENT *delayElement;
    UINT16 index;
    for(index = 0; index < TASKQUEUE_MAX_SIZE; index++){
        delayElement = &taskDelayList[index];
        if(delayElement->isValid){
            delayElement->counter--;
        }
    }
}
