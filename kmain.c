#include "sched.h"
#include "shell.h"
#include "kernel.h"
#include "syscall.h"
#include "keyboard.h"
#include "malloc.h"
#include "sem.h"
#include "kheap.h"

struct sem_s cmd_buffer_sem;
struct sem_s shell_sem;
char* cmd_buffer;

void kmain()
{
	init_kernel();
	
	create_process(&keyboard_loop);
	create_process(&start_shell);
	
	sem_init(&cmd_buffer_sem, 0);
	sem_init(&shell_sem, 0);
	cmd_buffer = (char*) kAlloc(sizeof(char) * 100);
	
	start_kernel();
	__asm("cps 0x10"); // CPU to USER mode

	while (1)
	{
		sys_yield();
	}
}
