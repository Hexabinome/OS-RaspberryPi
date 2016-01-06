#include "shell_commands.h"

#include "hw.h"
#include "process.h"
#include "sched.h"

extern struct pcb_s* current_process;

void do_echo(char* message)
{
	log_str(message);
}

void do_ps()
{
		
}

