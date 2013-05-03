#include <stdint.h>
#include <string.h>

#include "uckernel.h"



static uint8_t *commands[2] = {
	"help",
	"version"
};

uint16_t parse_result = 0;

extern void terminal_parse_command_callback(uckernel_task_event event, uckernel_task_data data);

static void help_command(void)
{
	serio_write_str("Available Commands:\r\n");
	serio_write_str("help\tPrints this message\r\n");
	serio_write_str("version\tPrints version number\r\n");
}

void parse_command(uckernel_task_event event, uckernel_task_data data)
{
	uint8_t i;

	if(strlen(data) == 0) goto valid;
	parse_result = 1;
	for (i = 0; i < sizeof(commands) / sizeof(uint8_t *); i++) {
		if (strcmp(data, commands[i]) == 0) {
			switch (i) {
			case 0: help_command();
				break;
			case 1: serio_write_str("valid command\r\n");
				break;
			default: break;
			}
			goto valid;
		}
	}
	serio_write_str("Unknown command. Type 'help' for list of commands\r\n");
valid:
	uckernel_submit_normal_task(terminal_parse_command_callback, NULL, &parse_result);
}
