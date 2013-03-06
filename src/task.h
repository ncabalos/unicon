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
    TASK_TASK2,
    TASK_MAX_SIZE
}TASK_ID;

typedef enum TASK_STATE_ENUM{
    TASK_DEAD,
    TASK_RUNNING,
    TASK_READY,
    TASK_WAITING
}TASK_STATE;

typedef void (*TASK_FUNCTION)(TASK_EVENT event, TASK_Q_ELEMENT data);

typedef void (*TASK)(void* data);

typedef struct TASK_ELEMENT_S{
    TASK task;
    void * data;
}TASK_ELEMENT;


typedef struct TASK_DELAY_ELEMENT_S{
    BOOL isValid;
    UINT16 counter;
    TASK_ELEMENT element;
}TASK_DELAY_ELEMENT;

typedef enum TASK_PRIORITY_E{
    TASK_PRIORITY_ISR,
    TASK_PRIORITY_NONTIMED,
    TASK_PRIORITY_MAX,
    TASK_PRIORITY_TIMED
}TASK_PRIORITY;

typedef struct TASK_QUEUE_S{
    UINT16 in;
    UINT16 out;
    UINT16 count;
    TASK_ELEMENT queue[TASKQUEUE_MAX_SIZE];
}TASK_QUEUE;

BOOL taskSubmit(TASK_PRIORITY priority, TASK task, void *data, UINT16 delay);
#define TASK_SUBMIT_ISR(t,d) taskSubmit(TASK_PRIORITY_ISR,t,d,NULL)
#define TASK_SUBMIT_TIMED(t,d,del) taskSubmit(TASK_PRIORITY_TIMED,t,d,del)
#define TASK_SUBMIT_NORMAL(t,d) taskSubmit(TASK_PRIORITY_NONTIMED,t,d,NULL)


//void taskSubmit_ISR(TASK task, void *data);
//void taskSubmit_Timed(TASK task, void *data, UINT16 delay, BOOL repeat);
//void taskSubmit(TASK task, void *data);
void taskProcess(void);
void taskProcessDelayList(void);




#endif	/* TASK_H */

