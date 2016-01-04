#include "syscall.h"
#include "sched.h"
#include "malloc.h"
#include "sched_irq.h"

static void child_stuff()
{
	uint32_t i;
	for (i = 0; i < 10000000; ++i) ;
}
static void child_stuff_2()
{
	uint32_t i;
	for (i = 0; i < 100000; ++i) ;
}

int process()
{
	int status, pid;
	
	pid = sys_fork();
	if (pid == 0)
	{
		// Child
		child_stuff();
		sys_exit(3);
	}
	else
	{
		// Parent
		sys_waitpid(pid, &status);
	}
	
	pid = sys_fork();
	if (pid == 0)
	{
		// Child
		child_stuff_2();
		sys_exit(13);
	}
	else
	{
		// Parent
		sys_waitpid(-1, &status);
	}
	
	pid = sys_fork();
	if (pid == 0)
	{
		// Child
		child_stuff_2();
		sys_exit(20);
	}
	else
	{
		// Parent
		int pid_2 = sys_fork();
		if (pid_2 == 0)
		{
			child_stuff();
			sys_exit(21);
		}
		else
		{
			sys_wait(&status);
			sys_wait(&status);
		}
	}
	
	// TODO test error (pid that doesn't exist)
	
	return 0;
}

void kmain()
{
	sched_init();
	
	create_process(&process);
	
	irq_init();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	while (1)
	{
		sys_yield();
	}
}

