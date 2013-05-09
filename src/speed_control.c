#include <stdint.h>
#include "tdef.h"
#include "uckernel.h"
#include "pid/pid.h"

static struct pid_info speed_control_pid;
static int16_t set_speed;
static int16_t measured_speed;
static uint16_t is_running;

static void speed_control_algorithm(uckernel_task_event event, uckernel_task_data data);
static void set_pwm_duty(uint16_t duty);

void speed_control_init(void)
{
	// initialize speed control hardware
	// initialize speed measurement hardware
	// initialize speed control mechanism
	pid_init(&speed_control_pid, 1, 0, 0, 100);
	is_running = false;
}

void speed_control_start(void)
{
	if(is_running == false){
		uckernel_submit_timed_task(speed_control_algorithm, NULL, NULL, speed_control_pid.interval);
		is_running = true;
	}
}

void speed_control_stop(void)
{
	is_running = false;
}

static void speed_control_algorithm(uckernel_task_event event, uckernel_task_data data)
{
	int16_t result;
	result = pid_update(&speed_control_pid, measured_speed);
	set_pwm_duty(result);
	if(is_running)
		uckernel_submit_timed_task(speed_control_algorithm, NULL, NULL, speed_control_pid.interval);
}

/**
 *
 * @param speed Speed in RPM. Positive is forward, negative is reverse
 */
void speed_control_set(int16_t speed)
{
	set_speed = speed;

}

static void set_pwm_duty(uint16_t duty)
{
	// scale duty to valid range
	// determine if need to add or minus
	// set duty cycle of pwm hardware

}






