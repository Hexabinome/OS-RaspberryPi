#include "sem.h"
#include "syscall.h"
#include "pwm.h"
#include "hw.h"
#include "kernel.h"

struct sem_s mutex;

int process1()
{
	sem_down(&mutex);
	sys_yield(); // long work where there will be two context switches
	sys_yield();
	sem_up(&mutex);
	
	return 0;
}


int process2()
{
	sem_down(&mutex);
	// do stuff
	sys_yield();
	sem_up(&mutex);
	
	return 0;
}

int process3()
{
	sem_down(&mutex);
	// do stuff
	sem_up(&mutex);
	
	return 0;
}

void kmain()
{
	init_kernel();
	
	create_process(&process1);
	create_process(&process2);
	create_process(&process3);
	
	start_kernel();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	sem_init(&mutex, 1);
	
	while (1)
	{
		sys_yield();
	}
}
