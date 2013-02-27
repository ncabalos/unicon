/* 
 * File:   task.h
 * Author: Nathaniel Abalos
 *
 * Created on February 26, 2013, 9:38 PM
 */

#ifndef TASK_H
#define	TASK_H

#define TASKLIST_MAX_SIZE 20
#define TASKQUEUE_MAX_SIZE 10

typedef UINT16 TASK_EVENT;

typedef void* TASK_DATA;

typedef void* TASK_Q_ELEMENT;

typedef enum TASK_ID_E{
    TASK_TIMER,
    TASK_TASK1,
    TASK_TASK2
}TASK_ID;

typedef enum TASK_STATE_ENUM{
    TASK_DEAD,
    TASK_RUNNING,
    TASK_READY,
    TASK_WAITING
}TASK_STATE;

typedef void (*TASK_FUNCTION)(TASK_EVENT event, TASK_Q_ELEMENT data);

typedef struct TASK_QUEUE_S{
    UINT16 queueIndexIn;
    UINT16 queueIndexOut;
    UINT16 queueCount;
    TASK_Q_ELEMENT data[TASKQUEUE_MAX_SIZE];
}TASK_Q;

typedef struct TASK_S{
    TASK_STATE state;
    TASK_FUNCTION function;
    TASK_Q eventQ;
    TASK_Q dataQ;
}TASK;

void taskSchedule(TASK_ID task, TASK_EVENT event, TASK_DATA data);
void taskRegister(TASK_ID task, TASK_FUNCTION function);
void taskUnschedule(void);
void taskProcess(void);

void taskEnqueue(TASK_Q *queue, TASK_Q_ELEMENT element);
TASK_Q_ELEMENT taskDequeue(TASK_Q *queue);


#endif	/* TASK_H */

