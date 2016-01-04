#include "sched.h"
#include "shell.h"

void kmain()
{
	sched_init();
	irq_init();
	
	create_process(&start_shell);
	
	__asm("cps 0x10"); // CPU to USER mode
	
	while (1) ;
}
