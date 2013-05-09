#include <stdint.h>
#include "pid.h"

void pid_init(struct pid_info * pid, int16_t p, int16_t i, int16_t d, uint16_t interval)
{
	pid->p = p;
	pid->i = i;
	pid->d = d;
	pid->interval = interval;
}
int16_t pid_update(struct pid_info * pid, int16_t current)
{

	int16_t p,i,d;

	pid->current = current;

        pid->p_error = pid->target - pid->current;
        pid->d_error = pid->p_error - pid->prev_error;
        pid->i_error = pid->i_error + pid->p_error;

        p = pid->p * pid->p_error;
        i = pid->i * pid->i_error;
        d = pid->d * pid->d_error;

        pid->result = p + i + d;

    return pid->result;
}
