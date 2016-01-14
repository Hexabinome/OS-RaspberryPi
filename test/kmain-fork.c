#include "syscall.h"
#include "sched.h"
#include "kernel.h"

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
	init_kernel();
	
	create_process(&process);
	
	start_kernel();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	while (1) ;
}
