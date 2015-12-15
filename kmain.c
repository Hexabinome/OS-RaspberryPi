#include "syscall.h"
#include "sched.h"
#include "util.h"

#define NB_PROCESS 5



void user_process_1()
{
	int v1 = 5;
	while (v1 != 4)
	{
		v1++;
	}
	v1++;
}

void user_process_2()
{
	int v2 = -12;
	while (v2 != 3)
	{
		v2 -= 1;
	}
	v2++;
}

void user_process_3()
{
	int v3 = 0;
	while (v3 != -1)
	{
		v3 += 1;
	}
	v3++;
}
void user_process_4()
{
	int v1 = 5;
	while (v1 != 4)
	{
		v1++;
	}
	v1++;
}
void user_process_5()
{
	int v1 = 5;
	while (v1 != 4)
	{
		v1++;
	}
	v1++;
}

void kmain()
{
	sched_init();
	
	// create_process((func_t*)&user_process_1);	
	// create_process((func_t*)&user_process_2);
	// create_process((func_t*)&user_process_3);
	// create_process((func_t*)&user_process_4);
	// create_process((func_t*)&user_process_5);

	create_process_with_fix_priority((func_t*)&user_process_1, 1);	
	create_process_with_fix_priority((func_t*)&user_process_2, 2);
	create_process_with_fix_priority((func_t*)&user_process_3, 3);
	create_process_with_fix_priority((func_t*)&user_process_4, 4);
	create_process_with_fix_priority((func_t*)&user_process_5, 5);

	
	/*
	create_process((func_t*)&user_process_6);
	create_process((func_t*)&user_process_7);
	create_process((func_t*)&user_process_8);
	create_process((func_t*)&user_process_9);
	create_process((func_t*)&user_process_10);	
	*/
	// Switch to user mode
	__asm("cps 0x10");

	while (1)
	{
		sys_yield();
	}
}
