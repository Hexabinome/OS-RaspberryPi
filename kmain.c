#include "syscall.h"
#include "sched.h"
#include "util.h"
#include "pwm.h"

#define NB_PROCESS 5

void user_process_1()
{
	for(;;)
	{
		playSound(1);
	}
}

void user_process_2()
{
	for(;;)
	{
		playSound(2);
	}
}

void user_process_3()
{
	for(;;)
	{
		playSound(3);
	}
}
void user_process_4()
{
	for(;;)
	{
		playSound(4);
	}
}
void user_process_5()
{
	for(;;)
	{
		playSound(5);
	}
}

void kmain()
{
	audio_init();
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
