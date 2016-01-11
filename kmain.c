#include "sched.h"
#include "shell.h"
#include "kernel.h"
#include "syscall.h"
#include "fb_cursor.h"
#include "hw.h"
#include "keyboard.h"

void LogPrint(char* message, uint32_t length)
{
	// Careful, this code is executed too early for us the first time
	// fb_address is still equal to 0, which means we erase the interruption vectors
	
  /*fb_print_text(">> ");
	fb_print_text(message);
	fb_print_char('\n');*/
}

void notice()
{
	fb_print_text("NOTICE");
}

static int keyboard()
{
	char c;
	while (1)
	{
		KeyboardUpdate();
		c = KeyboardGetChar();
		
		if (c != NULL)
		{
			if ((uint8_t) c == FB_BACKSPACE)
				fb_backspace();
			else if ((uint8_t) c == FB_DELETE)
				fb_delete();
			else if ((uint8_t) c == FB_ARROW_LEFT)
				fb_move_cursor_left();	
			else if ((uint8_t) c == FB_ARROW_RIGHT)
				fb_move_cursor_right();
			else
				fb_print_char(c);
		}
	}
	
	return 0;
}

void kmain()
{
	init_kernel();
	fb_print_text("Kernel init : OK\n");
	
	create_process(&keyboard);
	//create_process(&start_shell);
	
	//start_kernel();
	//fb_print_text("Kernel started\n");
	
	__asm("cps 0x10"); // CPU to USER mode

	while (1)
	{
		sys_yield();
	}
}
