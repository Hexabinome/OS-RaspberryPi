#include "sched.h"
#include "sched_irq.h"
#include "shell.h"

void kmain()
{
	sched_init();
	
	create_process(&start_shell);
	
	irq_init();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	while (1) ;
}
