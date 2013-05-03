#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "uckernel.h"

static uint8_t *ptr;
static uint8_t command_buffer[64];

extern void parse_command(uckernel_task_event event, uckernel_task_data data);

static void print_prompt(void)
{
	serio_write_str("% ");
}

void terminal_parse_command_callback(uckernel_task_event event, uckernel_task_data data)
{
	print_prompt();
}

void terminal_receive_handler(uckernel_task_event event, uckernel_task_data data)
{
	uint8_t msg[32];
	uint16_t cnt = serio_data_available();

	if (cnt) {
		serio_read_n(msg, cnt);
		serio_write_n(msg, cnt);
		memcpy(ptr, msg, cnt);
	}

	if (*ptr == '\r') {
		*ptr = '\0';
		ptr = command_buffer;
		serio_write_str("\r\n");
		uckernel_submit_normal_task(parse_command, NULL, command_buffer);
	} else {
		ptr++;
	}
}

void terminal_init(void)
{
	ptr = command_buffer;
	serio_write_str("\r\nUnicon Terminal, by Nathaniel Abalos\r\n");
	print_prompt();
}
