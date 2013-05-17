#include <stdint.h>
#include "drive_control.h"

void drive_control_init(void)
{
	speed_control_init();
	speed_control_start();
}
void drive_control_set_speed(uint16_t dir, uint16_t rpm)
{
	int16_t speed;
	switch(dir){
	case DRIVE_FORWARD:
		speed = (int16_t)rpm;
		break;
	case DRIVE_REVERSE:
		speed = (int16_t)rpm * -1;
		break;
	case DRIVE_PARK:
		speed = 0;
		break;
	}
	speed_control_set(speed);
}
void drive_control_set_steering(uint16_t dir, uint16_t value);
