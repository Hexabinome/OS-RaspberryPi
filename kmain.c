#include "sched.h"
#include "sched_irq.h"
#include "shell.h"

static void init()
{
	sched_init();
	
    create_process(&start_shell);
	irq_init();
}

void kmain()
{
	init();
	
	__asm("cps 0x10"); // CPU to USER mode

	while (1) ;
}
