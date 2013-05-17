#include <stdio.h>
#include <stdint.h>

#define USE_AND_OR
#include <p24Fxxxx.h>
#include <PIC24F_plib.h>
#include "tdef.h"
#include "queue.h"
#include "uckernel.h"
#include "startup.h"
#include "assert.h"

#define SERVOPWMTIMER_NUM 4
#define SERVOPWMTIMER_PRESCALE (8UL)

#if SERVOPWMTIMER_NUM == 2
#define SERVOPWMTIMER_ENABLE_INT IEC0bits.T2IE
#define SERVOPWMTIMER_INTERRUPT IFS0bits.T2IF
#define SERVOPWMTIMER_ENABLE T2CONbits.TON
#elif SERVOPWMTIMER_NUM == 4
#define SERVOPWMTIMER_ENABLE_INT IEC1bits.T4IE
#define SERVOPWMTIMER_INTERRUPT IFS1bits.T4IF
#define SERVOPWMTIMER_ENABLE T4CONbits.TON
#elif SERVOPWMTIMER_NUM == 6
#define SERVOPWMTIMER_ENABLE_INT TMR6IE
#define SERVOPWMTIMER_INTERRUPT TMR6IF
#define SERVOPWMTIMER_ENABLE TMR6ON
#endif

#if SERVOPWMTIMER_PRESCALE == 1
#define SERVOPWMTIMER_PRESCALE_BITS (0)
#elif SERVOPWMTIMER_PRESCALE == 8
#define SERVOPWMTIMER_PRESCALE_BITS (1)
#elif SERVOPWMTIMER_PRESCALE == 64
#define SERVOPWMTIMER_PRESCALE_BITS (2)
#elif SERVOPWMTIMER_PRESCALE == 256
#define SERVOPWMTIMER_PRESCALE_BITS (3)
#endif

//#define SERVOPWMTIMER_PR_VALUE (FREQ /(2UL * SERVOPWMTIMER_PRESCALE * 500000UL ))
#define SERVOPWMTIMER_PR_VALUE 19UL


static void servo_control_update_counters(uckernel_task_event event, uckernel_task_data data);
static void servo_control_process(uckernel_task_event event, uckernel_task_data data);

volatile uint16_t servo_period_count;
volatile uint16_t servo_pulse_count;
volatile uint8_t servo_channel_count;

uint16_t servo_pulse_value;
uint16_t servo_end_count;

#define CHANNEL0_PIN LATAbits.LATA0
#define CHANNEL1_PIN LATAbits.LATA1
#define CHANNEL2_PIN LATAbits.LATA2
#define CHANNEL3_PIN LATAbits.LATA3
#define CHANNEL4_PIN LATAbits.LATA3
#define CHANNEL5_PIN LATAbits.LATA3
#define CHANNEL6_PIN LATAbits.LATA3
#define CHANNEL7_PIN LATAbits.LATA3

enum servo_control_state {
	SERVOCONTROL_STATE_INIT,
	SERVOCONTROL_STATE_SETUP,
	SERVOCONTROL_STATE_PULSE,
	SERVOCONTROL_STATE_END
};

enum servo_control_event {
	SERVOCONTROL_EVENT_INIT,
	SERVOCONTROL_EVENT_READY,
	SERVOCONTROL_EVENT_TIMEREXPIRED
};

typedef void (*servo_pin_set_function)(uint8_t);

uint16_t servo_control_state = SERVOCONTROL_STATE_INIT;

static void servo_channel_0_set(uint8_t val)
{
	CHANNEL0_PIN = val & 0x01;
}

static void servo_channel_1_set(uint8_t val)
{
	CHANNEL1_PIN = val & 0x01;
}

static void servo_channel_2_set(uint8_t val)
{
	CHANNEL2_PIN = val & 0x01;
}

static void servo_channel_3_set(uint8_t val)
{
	CHANNEL3_PIN = val & 0x01;
}

static void servo_channel_4_set(uint8_t val)
{
	CHANNEL4_PIN = val & 0x01;
}

static void servo_channel_5_set(uint8_t val)
{
	CHANNEL5_PIN = val & 0x01;
}

static void servo_channel_6_set(uint8_t val)
{
	CHANNEL6_PIN = val & 0x01;
}

static void servo_channel_7_set(uint8_t val)
{
	CHANNEL7_PIN = val & 0x01;
}

static servo_pin_set_function pointers[8] = {
	servo_channel_0_set,
	servo_channel_1_set,
	servo_channel_2_set,
	servo_channel_3_set,
	servo_channel_4_set,
	servo_channel_5_set,
	servo_channel_6_set,
	servo_channel_7_set
};

void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void)
{
	_T4IF = 0;
	servo_control_process(SERVOCONTROL_EVENT_TIMEREXPIRED, NULL);
}

static void set_timer_pr(uint16_t pr)
{
	PR4 = pr * 2 - 10;
}

static void servopwm_timer_init(void)
{
#if SERVOPWMTIMER_NUM == 2
	T2CONbits.TCKPS = SERVOPWMTIMER_PRESCALE_BITS;
	PR2 = SERVOPWMTIMER_PR_VALUE;
#elif SERVOPWMTIMER_NUM == 4
	T4CONbits.TCKPS = SERVOPWMTIMER_PRESCALE_BITS;
	//IPC6bits.T4IP = 7;
#elif SERVOPWMTIMER_NUM == 6
	T6CONbits.T6CKPS = SERVOPWMTIMER_PRESCALE_BITS;
	T6CONbits.T6OUTPS = SERVOPWMTIMER_POSTSCALE_BITS;
	PR6 = SERVOPWMTIMER_PR_VALUE;
#endif

	SERVOPWMTIMER_INTERRUPT = 0;
	SERVOPWMTIMER_ENABLE_INT = 1;
}

static void servopwm_timer_set(uint8_t en)
{
	SERVOPWMTIMER_ENABLE = en & 0x01;
}

struct servo_info {
	uint8_t enabled;
	uint16_t pwm_duty;
};

#define SERVO_CHANNELS 8
#define SERVO_INTERVAL_COUNT 2500

struct servo_info servos[SERVO_CHANNELS];

struct servo_info *servo_ptr;

void servo_channel_enable(uint8_t channel)
{
	servos[channel].enabled = true;
}

void servo_channel_set_duty(uint8_t channel, uint16_t duty)
{
	servos[channel].pwm_duty = duty + 1000;
}

static void fill_servo_counters(uint16_t duty)
{
	// set up info
	servo_pulse_count = duty;
	servo_end_count = SERVO_INTERVAL_COUNT - duty;
}

void servo_control_init(void)
{
	servo_period_count = 0;
	servo_channel_count = 0;
	servo_pulse_count = 0;

	// setup timer here
	servopwm_timer_init();

	servo_control_state = SERVOCONTROL_STATE_SETUP;
	servo_channel_count = 0;
	servo_ptr = &servos[servo_channel_count];

	fill_servo_counters(servo_ptr->pwm_duty);

	uckernel_submit_isr_task(servo_control_process, SERVOCONTROL_EVENT_READY, NULL);

}

static void update_channel_counter(void)
{
	servo_channel_count++;
	servo_channel_count %= SERVO_CHANNELS;
}

static void servo_timer_off(void)
{
	servopwm_timer_set(false);
	TMR4 = 0;
}

static void servo_timer_reset(uint16_t pr)
{
	servo_timer_off();
	set_timer_pr(pr);
	servopwm_timer_set(true);
}

static void servo_control_process(uckernel_task_event event, uckernel_task_data data)
{
	servo_ptr = &servos[servo_channel_count];
	switch (servo_control_state) {
	case SERVOCONTROL_STATE_SETUP:
		switch (event) {
		case SERVOCONTROL_EVENT_READY:
			if (servo_ptr->enabled) {
				servo_control_state = SERVOCONTROL_STATE_PULSE;
				servo_timer_reset(servo_pulse_count);
				(pointers[servo_channel_count])(true);
			} else {
				servo_control_state = SERVOCONTROL_STATE_END;
				servo_timer_reset(SERVO_INTERVAL_COUNT);
				(pointers[servo_channel_count])(false);
			}
			break;
		}
		break;
	case SERVOCONTROL_STATE_PULSE:
		switch (event) {
		case SERVOCONTROL_EVENT_TIMEREXPIRED:
			servo_control_state = SERVOCONTROL_STATE_END;
			servo_timer_reset(servo_end_count);
			(pointers[servo_channel_count])(false);
			break;
		}
		break;
	case SERVOCONTROL_STATE_END:
		switch (event) {
		case SERVOCONTROL_EVENT_TIMEREXPIRED:
			servo_timer_off();
			(pointers[servo_channel_count])(false);
			servo_control_state = SERVOCONTROL_STATE_SETUP;
			update_channel_counter();

			servo_ptr = &servos[servo_channel_count];
			fill_servo_counters(servo_ptr->pwm_duty);

			uckernel_submit_isr_task(servo_control_process, SERVOCONTROL_EVENT_READY, NULL);
			break;
		}
		break;
	default: break;
	}
}

