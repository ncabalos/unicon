#define USE_AND_OR
#include <stdio.h>
#include <p24Fxxxx.h>
#include <PIC24F_plib.h>
#include "tdef.h"
#include "uckernel.h"
#include "startup.h"
#include "assert.h"

volatile uint32_t rpm_pulse_counter;
volatile uint32_t prev_pulse_counter;
uint8_t test[32];

volatile uint32_t rpm_period = 0UL;
volatile uint32_t rpm_frequency = 0UL;

volatile uint32_t overflow_count = 0UL;

volatile uint8_t last_pin = 0;

uint8_t rpm_running = false;
#define CRITICAL_SECTION_START asm volatile ("disi #0x3FFF");
#define CRITICAL_SECTION_END asm volatile ("disi #0");

void terminal_command_return_callback(uckernel_task_event event, uckernel_task_data data);

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{
	volatile uint8_t curr_pin;
	volatile uint32_t test1;
	volatile uint32_t test3;
	volatile uint16_t test2;
	curr_pin = PORTDbits.RD13;
	IFS1bits.CNIF = 0;
	if (curr_pin != last_pin && curr_pin) {
		test2 = TMR1;
		TMR1 = 0;
		test3 = overflow_count;
		overflow_count = 0UL;
		test1 = (test3 * 65535UL) + (uint32_t) test2;
		rpm_frequency = 200000000UL / test1;
		rpm_period = 10000000UL / rpm_frequency;
	}
	last_pin = curr_pin;
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
	IFS0bits.T1IF = 0;
	overflow_count++;
}

void rpm_print(uckernel_task_event event, uckernel_task_data data)
{
	sprintf(test, "rpm: %lu.%02luHz %lu.%02lums\r\n", rpm_frequency / 100UL,rpm_frequency % 100UL, rpm_period / 100UL, rpm_period % 100UL);
	serio_write_str(test);

	if (rpm_running)
		uckernel_submit_timed_task(rpm_print, NULL, NULL, 250);
	else
		uckernel_submit_timed_task(terminal_command_return_callback, NULL, NULL, 100);
}

void rpm_start(uckernel_task_event event, uckernel_task_data data)
{
	rpm_running = true;
	rpm_frequency = 0UL;
	rpm_period = 0UL;
	uckernel_submit_timed_task(rpm_print, NULL, NULL, 100);
}

void rpm_rx_handler(uckernel_task_event event, uckernel_task_data data)
{
	uint16_t cnt = serio_data_available();
	uint8_t c;
	if (cnt) {
		serio_read_n(&c, 1);
		rpm_running = false;
	}
}

void rpm_sensor_init(void)
{
	last_pin = PORTDbits.RD13;
	TRISDbits.TRISD13 = 1;
	CNEN2bits.CN19IE = 1;
	IFS1bits.CNIF = 0;
	IEC1bits.CNIE = 1;

	T1CON = 0x00; //Stops the Timer1 and reset control reg.
	TMR1 = 0x00; //Clear contents of the timer register
	PR1 = 0xFFFF; //Load the Period register with the value 0xFFFF
	IPC0bits.T1IP = 0x01; //Setup Timer1 interrupt for desired priority level
	// (This example assigns level 1 priority)
	IFS0bits.T1IF = 0; //Clear the Timer1 interrupt status flag
	IEC0bits.T1IE = 1; //Enable Timer1 interrupts
	T1CONbits.TON = 1; //Start Timer1 with prescaler settings at 1:1 and
	T1CONbits.TCKPS = 1;
	//clock source set to the internal instruction cycle
	/* Example code for Timer1 ISR*/

}
