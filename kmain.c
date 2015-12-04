#include "syscall.h"
#include "sched.h"
#include "util.h"
#include "vmem.h"

#define NB_PROCESS 5

void user_process_1()
{
	int v1 = 5;
	while (1)
	{
		v1++;
	}
}

void user_process_2()
{
	int v2 = -12;
	while (1)
	{
		v2 -= 2;
	}
}

void user_process_3()
{
	int v3 = 0;
	while (1)
	{
		v3 += 5;
	}
}

void kmain()
{
	int a = 0;
	sched_init();
	uint32_t pa = vmem_translate(&a,struct pcb_s* current_process);
}
