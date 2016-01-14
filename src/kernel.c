#include "kernel.h"

#include "kheap.h"
#include "hw.h"
#include "pwm.h"
#include "fb.h"
#include "vmem.h"
#include "sched.h"
#include "keyboard.h"
#include "fb_cursor.h"
#include "sem.h"

#include "sched_irq.h"

struct sem_s cmd_buffer_sem;
struct sem_s shell_sem;
char* cmd_buffer;


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
	fb_set_zoom(3);
	fb_display_info();
#endif

	sem_init(&cmd_buffer_sem, 0);
	sem_init(&shell_sem, 0);
	cmd_buffer = (char*) kAlloc(sizeof(char) * 100);
	
	fb_print_text("Kernel initialized\n");
}

void start_kernel()
{
	irq_init();
	
	fb_print_text("Kernel started\n");
}
