#include "sched.h"
#include "shell.h"
#include "kernel.h"
#include "syscall.h"
#include "keyboard.h"
#include "malloc.h"
#include "sem.h"
#include "kheap.h"

void kmain()
{
	init_kernel();
	
	create_process(&keyboard_loop);
	create_process(&start_shell);
	
	start_kernel();
	__asm("cps 0x10"); // CPU to USER mode

	while (1)
	{
		sys_yield();
	}
}
