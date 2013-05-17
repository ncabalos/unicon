#include "uckernel.h"

extern void terminal_command_return_callback(uckernel_task_event event, uckernel_task_data data);

void help_command(uckernel_task_event event, uckernel_task_data data)
{
	serio_write_str("---------- Available Commands ----------\r\n");
	serio_write_str("help               Prints this message  \r\n");
	serio_write_str("version            Prints version number\r\n");
	serio_write_str("servo [n] [m]      Set servo n to duty m\r\n");
	serio_write_str("rpm                Display RPM reading  \r\n");

	uckernel_submit_timed_task(terminal_command_return_callback, NULL, NULL, 100);
}
