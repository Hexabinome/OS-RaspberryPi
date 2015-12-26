#include "syscall.h"
#include "sched.h"
#include "malloc.h"

static int my_fork()
{
	return sys_fork();
}

int process()
{
	int* momo = (int*) gmalloc(5 * size(int));
	momo[0] = 15;
	momo[1] = 40;
	
	int pid = my_fork();
	if (pid == -1)
	{
		// failed, should not happen
	}
	else if (pid == 0)
	{
		// Child, display momo[0]
		while (1) ;
	}
	else
	{
		// parent
		momo[1] = 2;
		momo++;
		while (1) ;
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
