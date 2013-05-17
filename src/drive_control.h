/* 
 * File:   drive_control.h
 * Author: Nathaniel Abalos
 *
 * Created on May 8, 2013, 10:42 PM
 */

#ifndef DRIVE_CONTROL_H
#define	DRIVE_CONTROL_H

enum drive_direction{
	DRIVE_FORWARD,
	DRIVE_PARK,
	DRIVE_REVERSE
};

enum steer_direction{
	STEER_LEFT,
	STEER_CENTER,
	STEER_RIGHT
};

void drive_control_init(void);
void drive_control_set_speed(uint16_t dir, uint16_t rpm);
void drive_control_set_steering(uint16_t dir, uint16_t value);

// speed related
//void speed_control_init(void);
//void speed_control_start(void);
//void speed_control_stop(void);
//void speed_control_set(int16_t speed);

// steering related

#endif	/* DRIVE_CONTROL_H */

