#include "sched.h"
#include "sched_irq.h"
#include "shell.h"
#include "fb.h"
#include "hw.h"
#include "fb_cursor.h"

extern uint32_t pitch;;
/*extern uint32_t fb_x;
extern uint32_t fb_y;*/

void kmain()
{
	sched_init();
	
	hw_init();
	
uint32_t i;
	FramebufferInitialize();
	
	led_on();
	for (i = 0; i < 1000; ++i);
	
	fb_print_text("HELLO WORLD\n");
	/*fb_print_int(pitch);
	fb_print_char('\n');
	fb_print_int(fb_x);
	fb_print_char('\n');
	fb_print_int(fb_y);*/
	fb_print_text("\nCOUCOUC");
	
	led_off();
	for (i = 0; i < 1000; ++i) ;
	drawBlue();
	led_on();
	while(1);
	
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
