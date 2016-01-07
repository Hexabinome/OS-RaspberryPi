#include "sched.h"
#include "sched_irq.h"
#include "shell.h"
#include "fb.h"
#include "hw.h"

extern struct pcb_s* current_process;

void kmain()
{
	sched_init();
	
	hw_init();
	FramebufferInitialize();

	create_process(&start_shell);
	
	irq_init();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	while (1) ;
}
