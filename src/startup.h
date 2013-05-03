/* 
 * File:   startup.h
 * Author: Nathaniel Abalos
 *
 * Created on April 29, 2013, 10:07 PM
 */

#ifndef STARTUP_H
#define	STARTUP_H

#ifdef	__cplusplus
extern "C" {
#endif

	void steering_control_init(void);
	void drive_control_init(void);
	void serio_init(uckernel_task handler);


#ifdef	__cplusplus
}
#endif

#endif	/* STARTUP_H */

