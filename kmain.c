#include "syscall.h"
#include "vmem.h"
#include "config.h"
#include "pwm.h"

int process1()
{
	int a = 0;
	while (1)
	{
		a++;
	}
	return a;
}

void kmain()
{
	audio_init();
	sched_init();
	
	create_process(&process1);
	
	__asm("cps 0x10"); // CPU to USER mode

	uint32_t size = sizeof(int) * 10;
	int* momo = (int*)sys_mmap(size);

	momo[0] = 1;
	momo[4] = 4;
	momo[9] = 9;
	
	sys_munmap(momo, size);
	
	int i = 0;
	while (1)
	{
		i--;
	}
}
