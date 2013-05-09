/* 
 * File:   pid.h
 * Author: Nathaniel Abalos
 *
 * Created on May 2, 2013, 10:57 PM
 */

#ifndef PID_H
#define	PID_H

struct pid_info {
	int16_t p;
	int16_t i;
	int16_t d;
	int16_t target;
	int16_t current;
	int16_t p_error;
	int16_t i_error;
	int16_t d_error;
	int16_t prev_error;
	int16_t result;
	uint16_t interval;
};

void pid_init(struct pid_info * pid, int16_t p, int16_t i, int16_t d, uint16_t interval);
int16_t pid_update(struct pid_info * pid, int16_t current);



#endif	/* PID_H */

