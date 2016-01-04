#include "syscall.h"
#include "sched.h"
#include "sched_irq.h"

int process()
{
	int32_t cpt = 10;
	
	int pid = sys_fork();
	if (pid == -1)
	{
		// failed, should not happen
	}
	else if (pid == 0)
	{
		// Child
		cpt--;
		sys_nop();
	}
	else
	{
		// Parent
		cpt++;
		sys_nop();
	}
	
	return 0;
}

void kmain()
{
	sched_init();
	
	create_process(&process);
	
	irq_init();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	while (1) ;
}
