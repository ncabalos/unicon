/* 
 * File:   main.c
 * Author: Nathaniel Abalos
 *
 * Created on November 12, 2012, 9:47 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <p24Fxxxx.h>
#include <GenericTypeDefs.h>
#include "task.h"

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

#define USE_PLL 1
#define TICKTIMER_NUM 2
#define TICKTIMER_PRESCALE (8UL)

#if USE_PLL == 1
#define FREQ 32000000
#else
#define FREQ 8000000
#endif

#if TICKTIMER_NUM == 2
#define TICKTIMER_ENABLE_INT IEC0bits.T2IE
#define TICKTIMER_INTERRUPT IFS0bits.T2IF
#define TICKTIMER_ENABLE T2CONbits.TON
#elif TICKTIMER_NUM == 4
#define TICKTIMER_ENABLE_INT TMR4IE
#define TICKTIMER_INTERRUPT TMR4IF
#define TICKTIMER_ENABLE TMR4ON
#elif TICKTIMER_NUM == 6
#define TICKTIMER_ENABLE_INT TMR6IE
#define TICKTIMER_INTERRUPT TMR6IF
#define TICKTIMER_ENABLE TMR6ON
#endif

#if TICKTIMER_PRESCALE == 1
#define TICKTIMER_PRESCALE_BITS (0)
#elif TICKTIMER_PRESCALE == 8
#define TICKTIMER_PRESCALE_BITS (1)
#elif TICKTIMER_PRESCALE == 64
#define TICKTIMER_PRESCALE_BITS (2)
#elif TICKTIMER_PRESCALE == 256
#define TICKTIMER_PRESCALE_BITS (3)
#endif

#define TICKTIMER_PR_VALUE (FREQ /(2UL * TICKTIMER_PRESCALE * 1000UL ))


volatile UINT16 msCount = 1000;


void T2Interrupt_ISR(void)
{
    _T2IF = 0;
    if(!msCount--)
    {
        msCount = 1000;
        taskSchedule(TASK_TIMER, 0x01, 0xAA);
    }
    /* For delay_ms() */
//    if(!delayCount--) delayExpire = TRUE;
//    lcdRefresh();
}

void _ISR _T2Interrupt(void)
{
    T2Interrupt_ISR();
}

void timerTask(TASK_EVENT event, TASK_Q_ELEMENT data)
{

    LATAbits.LATA1 = !LATAbits.LATA1;
}

void task1(TASK_EVENT event, TASK_Q_ELEMENT data)
{
    LATAbits.LATA0 = 1;
    taskSchedule(TASK_TASK2, 0x01, 0x00);
}

void task2(TASK_EVENT event, TASK_Q_ELEMENT data)
{
    LATAbits.LATA0 = 0;
    taskSchedule(TASK_TASK1, 0x0A, 0x55);
}

void tickTimerSetup(void)
{

#if TICKTIMER_NUM == 2
    T2CONbits.TCKPS = TICKTIMER_PRESCALE_BITS;
    PR2 = TICKTIMER_PR_VALUE;
#elif TICKTIMER_NUM == 4
    T4CONbits.T4CKPS = TICKTIMER_PRESCALE_BITS;
    T4CONbits.T4OUTPS = TICKTIMER_POSTSCALE_BITS;
    PR4 = TICKTIMER_PR_VALUE;
#elif TICKTIMER_NUM == 6
    T6CONbits.T6CKPS = TICKTIMER_PRESCALE_BITS;
    T6CONbits.T6OUTPS = TICKTIMER_POSTSCALE_BITS;
    PR6 = TICKTIMER_PR_VALUE;
#endif

    TICKTIMER_INTERRUPT = 0;
    TICKTIMER_ENABLE_INT = 1;
}
void tickTimerStart(void)
{
    TICKTIMER_ENABLE = 1;
}

int main(void) {

    tickTimerSetup();
    tickTimerStart();

    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;

    taskRegister(TASK_TIMER,timerTask);
    taskRegister(TASK_TASK1,task1);
    taskRegister(TASK_TASK2,task2);
//
    taskSchedule(TASK_TASK1, 0x33, 0x11);

    while(1){
        taskProcess();
    }
    return (EXIT_SUCCESS);
}




