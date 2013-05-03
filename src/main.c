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
#include "datastructure.h"
//#include "queue.h"
#include "stack.h"

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
_CONFIG1(WDTPS_PS32768 & FWPSA_PR128 & WINDIS_OFF & FWDTEN_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF & BKBUG_ON)

extern void terminal_receive_handler(uckernel_task_event event, uckernel_task_data data);


void layer2_init(uckernel_task_event event, uckernel_task_data data)
{
	terminal_init();
}

void task2(uckernel_task_event event, uckernel_task_data data)
{
	LATAbits.LATA1 = !LATAbits.LATA1;
	uckernel_submit_timed_task(task2, NULL, NULL, 200);
}

#define CRITICAL_SECTION_START asm volatile ("disi #0x3FFF");
#define CRITICAL_SECTION_END asm volatile ("disi #0");

int main(void)
{
	tick_timer_init();
	tick_timer_start();

	TRISAbits.TRISA0 = 0;
	TRISAbits.TRISA1 = 0;

	serio_init(terminal_receive_handler);

	uckernel_init();

	uckernel_submit_timed_task(layer2_init, NULL, NULL, 1000);

	while (1) {
		uckernel_process_tasks();
	}
	return(EXIT_SUCCESS);
}




