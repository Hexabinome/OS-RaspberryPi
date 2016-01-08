#include "sched.h"
#include "sched_irq.h"
#include "shell.h"
#include "fb.h"
#include "hw.h"
#include "fb_cursor.h"

extern int32_t fb_phy_address;
void kmain()
{
	sched_init();
	
	hw_init();
	FramebufferInitialize();
	int i;
	
	led_on();
	for (i = 0; i < 1000; ++i);
	
	fb_print_text("HELLO WORLD\n");
	//fb_print_int(fb_phy_address);
	fb_print_text("\nCOUCOUC");
	
	led_off();
	for (i = 0; i < 1000; ++i) ;
	drawBlue();
	while(1);
	
	while (1)
	{
		drawBlue();
		for (i = 0; i < 1000000; ++i) ;
		//fb_print_text("Hello world");
	}

	create_process(&start_shell);
	
	create_process(&start_shell);
	irq_init();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	while (1) ;
}
