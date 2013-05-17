#include <stdint.h>
#include "tdef.h"
#include "uckernel.h"
#include "pid/pid.h"

static struct pid_info speed_control_pid;
static int16_t set_speed;
static int16_t measured_speed;
static uint16_t is_running;

static void speed_control_algorithm(uckernel_task_event event, uckernel_task_data data);
static void set_servo_duty(int16_t duty);

void speed_control_init(void)
{
	// initialize speed control hardware
	servo_channel_enable(0);
	set_servo_duty(500);
	//servo_channel_set_duty(0, 0);
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
	set_servo_duty(result);
	if(is_running)
		uckernel_submit_timed_task(speed_control_algorithm, NULL, NULL, speed_control_pid.interval);
}

/**
 *
 * @param speed Speed in RPM. Positive is forward, negative is reverse
 */
void speed_control_set(int16_t speed)
{
	speed_control_pid.target = speed;
}

static void set_servo_duty(int16_t duty)
{
	uint16_t d;

	d = 500;

	if(duty < -500) duty = -500;
	if(duty > 500) duty = 500;

	d += duty;

	if(duty > 1000) duty = 1000;
	servo_channel_set_duty(0, duty);
}
