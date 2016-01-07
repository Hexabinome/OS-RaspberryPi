#include "sched.h"
#include "sched_irq.h"
#include "shell.h"
#include "fb.h"
#include "hw.h"

extern struct pcb_s* current_process;

void kmain()
{
	sched_init();

	char* cmd = "hello    my name  is         david   \n";
	int argc;
	char** res = parse_command(cmd, &argc);
	return;
}

