#include "syscall.h"
#include "vmem.h"
#include "config.h"
#include "pwm.h"
#include "asm_tools.h"
#include "hw.h"

extern struct pcb_s* current_process;
int process1()
{
	int* momo1 = (int*)sys_mmap(sizeof(int) * 10);
	int a1 = vmem_translate((uint32_t)momo1, current_process);
	sys_yield();
	
	sys_munmap(momo1, sizeof(int) * 10);
	a1 = vmem_translate((uint32_t)momo1, current_process);
	
	momo1[40000] = 50; // error : data_handler
	momo1++;
	
	return a1;
}


int process2()
{
	int* momo2 = (int*)sys_mmap(sizeof(int) * 10);
	int a2 = vmem_translate((uint32_t)momo2, current_process);
	sys_yield();
	
	sys_munmap(momo2, sizeof(int) * 10);
	a2 = vmem_translate((uint32_t)momo2, current_process);
	
	momo2++;
	return a2;
}

void kmain()
{
	audio_init();
	sched_init();

	create_process(&process1);
	create_process(&process2);
	
	irq_init();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	int i = 0;
	while (1)
	{
		++i;
	}
}
