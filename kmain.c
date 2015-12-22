#include "syscall.h"
#include "sched.h"
#include "pwm.h"
#include "malloc.h"

static int process1()
{
	int* momo = (int*)gmalloc(sizeof(int) * 10);
	momo[0] = 1;
	momo[4] = 4;
	momo[9] = 9;
	
	gfree(momo);
	
	momo = (int*)gmalloc(sizeof(int) * 10);
	momo[0] = 1;
	momo[4] = 4;
	momo[10] = 9; // should fail
	
	return 0;
}

void kmain()
{
	audio_init();
	sched_init();

	create_process(&process1);
	irq_init();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	while (1)
	{
		sys_yield();
	}
}
