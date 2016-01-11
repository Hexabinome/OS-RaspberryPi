#include "kernel.h"

#include "kheap.h"
#include "hw.h"
#include "pwm.h"
#include "fb.h"
#include "vmem.h"
#include "sched.h"
#include "keyboard.h"
#include "fb_cursor.h"

#include "sched_irq.h"

void init_kernel()
{
	kheap_init();
	hw_init();
	audio_init();
	UsbInitialise();
	
#if FB
	FramebufferInitialize();
#endif
	
#if VMEM
	vmem_init();
#endif

	sched_init();

#if FB
	fb_set_zoom(2);
	fb_display_info();
#endif


	fb_print_text("Kernel initialized\n");
}

void start_kernel()
{
	irq_init();
	__asm("cps 0x10"); // CPU to USER mode
	
	fb_print_text("Kernel started\n");
}
