#define USE_AND_OR
#include <stdint.h>
#include <p24Fxxxx.h>
#include <GenericTypeDefs.h>
#include "assert.h"
#include "system.h"
#include "uckernel.h"
#include "PIC24F_plib.h"

#define TICKTIMER_NUM 2
#define TICKTIMER_PRESCALE (8UL)

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

volatile uint16_t msCount = 1000;
volatile uint32_t tick_count = 0UL;

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
{
	_T2IF = 0;
	tick_count++;
	uckernel_process_delayed_task_list(NULL, NULL);
	LATAbits.LATA4 = !LATAbits.LATA4;
	assert(_T2IF == 0);
}

void tick_timer_init(void)
{

#if TICKTIMER_NUM == 2
	T2CONbits.TCKPS = TICKTIMER_PRESCALE_BITS;
	PR2 = TICKTIMER_PR_VALUE;
	IPC1bits.T2IP = 7;
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

void tick_timer_start(void)
{
	TICKTIMER_ENABLE = 1;
}
