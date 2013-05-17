/** 
 * @file uckernel.h
 * @author Nathaniel Abalos
 * @date 02/26/2013
 */

#ifndef UCKERNEL_H
#define	UCKERNEL_H

#include <stdint.h>

/** Maximum queue size */
#define TASKQUEUE_MAX_SIZE 10

/** Task priority enumerations */
enum uckernel_task_priority {
	/** High priority task */
	UCKERNEL_TASK_PRIORITY_HIGH,
	/** Normal priority task */
	UCKERNEL_TASK_PRIORITY_NORMAL,
	/** Maximum number of priorities */
	UCKERNEL_TASK_PRIORITY_MAX,
	/** Timed priority task */
	UCKERNEL_TASK_PRIORITY_TIMED
};

/** Task priority datatype */
typedef enum uckernel_task_priority uckernel_task_priority;
/** Task event datatype */
typedef uint16_t uckernel_task_event;
/** Task data datatype */
typedef void* uckernel_task_data;
/** Task function pointer datatype */
typedef void (*uckernel_task)(uckernel_task_event event, uckernel_task_data data);

BOOL uckernel_submit_isr_task(uckernel_task task, uckernel_task_event event, uckernel_task_data data);
BOOL uckernel_submit_timed_task(uckernel_task task, uckernel_task_event event, uckernel_task_data data, uint16_t delay);
BOOL uckernel_submit_normal_task(uckernel_task task, uckernel_task_event event, uckernel_task_data data);

void uckernel_init(void);

/**
 * This function needs to be called at regular intervals within the main loop,
 * as follows:
 * @code
 * while(1){
 *      uckernel_process_tasks();
 * }
 * @endcode
 */
void uckernel_process_tasks(void);
/**
 * This function updates the counter for each of the timed tasks in the list. If 
 * the counter for a timed task expires, it will be submitted to the kernel as a
 * normal task. This function is in itself a task that gets submitted after each
 * timer tick. This is the reason it has the same function parameters as a task
 * that would be submitted to the kernel. For example, it can be called from a
 * timer interrupt as follows:
 * \code
 * uckernel_submit_isr_task(uckernel_process_delayed_task_list, NULL, NULL);
 * \endcode
 * @param event Not used in this function
 * @param data Not used in this function
 */
void uckernel_process_delayed_task_list(uckernel_task_event event, uckernel_task_data data);

#endif	/* UCKERNEL_H */

