#include "sched.h"
#include "sched_irq.h"
#include "shell.h"
#include "fb.h"
#include "hw.h"
#include "fb_cursor.h"

extern int32_t fb_phy_address;
<<<<<<< HEAD
=======

>>>>>>> working frame when no mmu
void kmain()
{
	sched_init();
	
	hw_init();
	
uint32_t i;
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
	
<<<<<<< HEAD
=======
	led_on();
	for (i = 0; i < 10000; ++i) ;
	
	fb_print_text("Hello world\n");
	fb_print_int(fb_phy_address);
	fb_print_text("\nBLABLABLALBALBLAALBALBLABLLBALABLBLALBLBALLBALBLABLLBALABLABLALBLBALBLLABLALB");
	for (i = 0; i < 10000; ++i) ;
	//draw();
	
	led_off();
	for (i = 0; i < 10000; ++i) ;
	while(1) ;
	
	
	for (i = 0; i < 1000000000; ++i) ;
	
>>>>>>> working frame when no mmu
	while (1)
	{
		led_off();
		for (i = 0; i < 1000000000; ++i) ;
		draw();
		led_on();
		for (i = 0; i < 1000000000; ++i) ;
		//fb_print_text("Hello world");
	}

	create_process(&start_shell);
	
	create_process(&start_shell);
	irq_init();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	while (1) ;
}
