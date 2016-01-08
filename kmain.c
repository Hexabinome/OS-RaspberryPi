#include "sched.h"
#include "sched_irq.h"
#include "shell.h"
#include "fb.h"
#include "hw.h"
#include "kheap.h"

static void init()
{
	kheap_init();
	hw_init();
	FramebufferInitialize();
	sched_init();
	
    create_process(&start_shell);
	irq_init();
}

void kmain()
{
	init();
	
	__asm("cps 0x10"); // CPU to USER mode

	while (1) ;
}
