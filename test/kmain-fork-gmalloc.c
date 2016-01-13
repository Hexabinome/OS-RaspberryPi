#include "syscall.h"
#include "sched.h"
#include "malloc.h"
#include "kernel.h"

static int my_fork()
{
	return sys_fork();
}

int process()
{
	int* momo = (int*) gmalloc(5 * sizeof(int));
	momo[0] = 15;
	momo[1] = 40;	
	
	int pid = my_fork();
	if (pid == -1)
	{
		// failed, should not happen
	}
	else if (pid == 0)
	{
		// Child
		momo[1] = 5;
		sys_yield();
	}
	else
	{
		// Parent
		momo[1] = 2;
		momo++;
		sys_yield();
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
