/* 
 * File:   system.h
 * Author: Nathaniel Abalos
 *
 * Created on May 6, 2013, 9:34 PM
 */

#ifndef SYSTEM_H
#define	SYSTEM_H

#define USE_PLL 1

#if USE_PLL == 1
#define FREQ 32000000
#else
#define FREQ 8000000
#endif

#endif	/* SYSTEM_H */

