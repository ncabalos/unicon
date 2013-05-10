/* 
 * File:   main.c
 * Author: Nathaniel Abalos
 *
 * Created on November 12, 2012, 9:47 PM
 */
#define USE_AND_OR
#include <stdio.h>
#include <stdlib.h>
#include <p24Fxxxx.h>
#include <GenericTypeDefs.h>
#include "uckernel.h"
#include "PIC24F_plib.h"
#include "tdef.h"
#include "datastructure.h"
//#include "queue.h"
#include "stack.h"
#include "assert.h"

// CONFIG3
// Write Protection Flash Page Segment Boundary (Highest Page (same as page 170))
// Segment Write Protection Disable bit (Segmented code protection disabled)
// Configuration Word Code Page Protection Select bit (Last page(at the top of program memory) and Flash configuration words are not protected)
// Segment Write Protection End Page Select bit (Write Protect from WPFP to the last page of memory)
_CONFIG3(WPFP_WPFP511 & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM)

// CONFIG2
// Primary Oscillator Select (XT oscillator mode selected)
// Internal USB 3.3V Regulator Disable bit (Regulator is disabled)
// IOLOCK One-Way Set Enable bit (Write RP Registers Once)
// Primary Oscillator Output Function (OSCO functions as CLKO (FOSC/2))
// Clock Switching and Monitor (Both Clock Switching and Fail-safe Clock Monitor are disabled)
// Oscillator Select (Primary oscillator (XT, HS, EC) with PLL module (XTPLL,HSPLL, ECPLL))
// 96MHz PLL Disable (Enabled)
// USB 96 MHz PLL Prescaler Select bits (Oscillator input divided by 2 (8MHz input))
// Internal External Switch Over Mode (IESO mode (Two-speed start-up)disabled)
_CONFIG2(POSCMOD_XT & DISUVREG_OFF & IOL1WAY_ON & OSCIOFNC_OFF & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL_96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)

// CONFIG1
// Watchdog Timer Postscaler (1:32,768)
// WDT Prescaler (Prescaler ratio of 1:128)
// Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
// Watchdog Timer Enable (Watchdog Timer is disabled)
// Comm Channel Select (Emulator functions are shared with PGEC2/PGED2)
// General Code Segment Write Protect (Writes to program memory are allowed)
// General Code Segment Code Protect (Code protection is disabled)
// JTAG Port Enable (JTAG port is disabled)
_CONFIG1(WDTPS_PS256 & FWPSA_PR128 & WINDIS_OFF & FWDTEN_ON & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF & BKBUG_ON)

extern void terminal_receive_handler(uckernel_task_event event, uckernel_task_data data);

void module_layer_init(uckernel_task_event event, uckernel_task_data data)
{
	terminal_init();
}

void startup_error(uckernel_task_event event, uckernel_task_data data)
{
	TRISAbits.TRISA4 = 0;
	LATAbits.LATA4 = 1;
}

void hil_layer_init(uckernel_task_event event, uckernel_task_data data)
{
	serio_init(terminal_receive_handler);
	servo_control_init();

	servo_channel_enable(1);
	servo_channel_set_duty(1, 500);

	servo_channel_enable(2);
	servo_channel_set_duty(2, 1000);

	uckernel_submit_normal_task(module_layer_init, NULL, NULL);
}

#define CRITICAL_SECTION_START asm volatile ("disi #0x3FFF");
#define CRITICAL_SECTION_END asm volatile ("disi #0");

int main(void)
{
	uint16_t error_occured = false;
	// Enable Watchdog timer
	EnableWDT(WDT_ENABLE);
	// Check if watchdog timer expired
	if (RCONbits.WDTO) {
		RCONbits.WDTO = 0;
		error_occured = true;
	}

	TRISAbits.TRISA0 = 0;
	TRISAbits.TRISA1 = 0;
	TRISAbits.TRISA2 = 0;
	TRISAbits.TRISA3 = 0;

	tick_timer_init();
	tick_timer_start();

	uckernel_init();

	uckernel_submit_timed_task(hil_layer_init, NULL, NULL, 100);
	if (error_occured)
		uckernel_submit_timed_task(startup_error, NULL, NULL, 50);

	while (1) {
		uckernel_process_tasks();
		// kick the dog!
		ClrWdt();
	}
	return(EXIT_SUCCESS);
}




