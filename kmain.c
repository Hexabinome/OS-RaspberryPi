#include "sched.h"
#include "shell.h"

void kmain()
{
	init_kernel();
	
	create_process(&start_shell);
	
	start_kernel();
	
	__asm("cps 0x10"); // CPU to USER mode

	while (1) ;
}
