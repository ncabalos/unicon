#include <stdint.h>
#include <string.h>
#include "ctype.h"
#include "tdef.h"
#include "uckernel.h"

static uint8_t *commands[] = {
	"help",
	"version",
	"servo"
};

uint16_t parse_result = 0;

extern void terminal_parse_command_callback(uckernel_task_event event, uckernel_task_data data);

static void help_command(void)
{
	serio_write_str("Available Commands:\r\n");
	serio_write_str("help\tPrints this message\r\n");
	serio_write_str("version\tPrints version number\r\n");
	serio_write_str("servo [n] [m]\tSet servo n to duty m\r\n");
}

static uint16_t search_command_list(uint8_t * cmd)
{
	uint16_t i;
	for (i = 0; i < sizeof(commands) / sizeof(uint8_t *); i++) {
		if (strcmp(cmd, commands[i]) == 0) {
			break;
		}
	}
	return i;
}

uint16_t servo_cmd_param[2];

static uint16_t is_number(uint8_t *s)
{
	uint16_t len;
	len = strlen(s);
	while(len--){
		if(isdigit(*s++) == 0) return false;
	}
	return true;
}

static uint16_t parse_servo_command_parameter(void)
{
	char * pch;
	uint16_t *pint;
	pch = strtok(NULL, " ");
	pint = servo_cmd_param;
	while(pch){
		if(is_number(pch) == false)
			return false;
		*pint = atoi(pch);
		pint++;
		pch = strtok(NULL, " ");
	}
	servo_channel_set_duty(servo_cmd_param[0],servo_cmd_param[1]);
	return true;
}

void parse_command(uckernel_task_event event, uckernel_task_data data)
{
	uint16_t cmd_index;
	uint16_t res;
	char * pch;

	if (strlen(data) == 0) goto valid;
	parse_result = 1;

	pch = strtok(data, " ");
	cmd_index = search_command_list(pch);
	switch (cmd_index) {
	case 0:
		help_command();
		goto valid;
	case 1:
		serio_write_str("valid command\r\n");
		goto valid;
	case 2:
		serio_write_str("servo command\r\n");
		res = parse_servo_command_parameter();
		if (res)
			goto valid;
		break;
	default:
		break;
	}

	serio_write_str("Unknown command. Type 'help' for list of commands\r\n");
valid:
	uckernel_submit_normal_task(terminal_parse_command_callback, NULL, &parse_result);
}
