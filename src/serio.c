#include <stdio.h>
#include <stdint.h>

#define USE_AND_OR
#include <p24Fxxxx.h>
#include <PIC24F_plib.h>
#include "queue.h"
#include "uckernel.h"
#include "startup.h"
#include "assert.h"

#define QUEUE_SIZE 300
static uint8_t tx_queue_data[QUEUE_SIZE];
static struct queue tx_queue;
static uint8_t rx_queue_data[QUEUE_SIZE];
static struct queue rx_queue;

static uckernel_task rx_handler;

static void get_data(uint8_t * data, uint16_t n)
{
	while (n--) {
		dequeue(&rx_queue, data++);
	}
}

void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void)
{
	static uint8_t i = 0;

	U2TX_Clear_Intr_Status_Bit;
	if (get_count(&tx_queue)) {
		while (U2STAbits.UTXBF == 0 && get_count(&tx_queue) > 0) {
			if (!U2STAbits.UTXBF) {
				dequeue(&tx_queue, &i);
				WriteUART2(i);
			}
		}
	}
}

void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void)
{
	static uint8_t c = 0;
	U2RX_Clear_Intr_Status_Bit;
	while (!DataRdyUART2());
	c = ReadUART2();
	enqueue(&rx_queue, &c);
	if (rx_handler) {
		uckernel_submit_isr_task(rx_handler, NULL, NULL);
	}
	LATAbits.LATA0 = !LATAbits.LATA0;
}

void serio_set_rx_handler(uckernel_task handler)
{
	rx_handler = handler;
}

void serio_init(uckernel_task handler)
{
	serio_set_rx_handler(handler);

	new_queue(&tx_queue, tx_queue_data, QUEUE_SIZE, sizeof(uint8_t));
	new_queue(&rx_queue, rx_queue_data, QUEUE_SIZE, sizeof(uint8_t));

	iPPSInput(IN_FN_PPS_U2RX, IN_PIN_PPS_RP10);
	iPPSInput(IN_FN_PPS_U2CTS, IN_PIN_PPS_RPI32);
	iPPSOutput(OUT_PIN_PPS_RP17, OUT_FN_PPS_U2TX);
	iPPSOutput(OUT_PIN_PPS_RP31, OUT_FN_PPS_U2RTS);
	CloseUART2();

	/*Enable UART intruupts*/
	ConfigIntUART2(UART_RX_INT_EN | UART_RX_INT_PR6 | UART_TX_INT_EN | UART_TX_INT_PR6 | UART_INT_TX_BUF_EMPTY);
	/*UART initialized to 9600 baudrate @BRGH=0, 8bit,no parity and 1 stopbit*/
	OpenUART2(UART_EN | UART_BRGH_FOUR, UART_TX_ENABLE, 34);
}

void serio_write_n(uint8_t * data, uint16_t len)
{
	while (len--) {
		enqueue(&tx_queue, data++);
	}
	if (!IFS1bits.U2TXIF)
		IFS1bits.U2TXIF = 1;
}

void serio_write_term(uint8_t * data, uint8_t term)
{
	while (*data != term) {
		enqueue(&tx_queue, data++);
	}
	IFS1bits.U2TXIF = 1;

}

void serio_write_str(uint8_t * data)
{
	serio_write_term(data, '\0');
}

void serio_read_all(uint8_t *data)
{
	get_data(data, rx_queue.count);
}

void serio_read_n(uint8_t * data, uint16_t n)
{
	get_data(data, n);
}

uint16_t serio_data_available(void)
{
	return get_count(&rx_queue);
}

