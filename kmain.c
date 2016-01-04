#include "sched.h"
#include "shell.h"

void kmain()
{
	sched_init();
	irq_init();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	start_shell();
}
