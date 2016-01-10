#include "sched.h"
#include "shell.h"
#include "kernel.h"
#include "syscall.h"
#include "fb_cursor.h"
#include "hw.h"
#include "keyboard.h"

void LogPrint(char* message, uint32_t length)
{
	fb_print_text(">> ");
	fb_print_text(message);
	fb_print_char('\n');
}

void notice()
{
	fb_print_text("NOTICE");
}

static void temp()
{
	uint32_t i;
	for (i = 0; i < 500000; ++i)
		;
}

/*static int keyboard()
{
	char c;
	while (1)
	{
		blink();
		KeyboardUpdate();
		blink();
		c = KeyboardGetChar();
		
		if (c != NULL)
		{
			fb_print_char(c);
		}
	}
	
	return 0;
}*/

void kmain()
{
	init_kernel();
	fb_print_text("Kernel init : OK\n");
	
	//create_process(&keyboard);
	//create_process(&start_shell);
	
	//start_kernel();
	fb_print_text("Kernel started\n");
	
	char c;
	while (1)
	{
		KeyboardUpdate();
		c = KeyboardGetChar();
		if (c != NULL)
		{
			fb_print_char(c);
		}
		else
		{			
			fb_print_char(c);
		}
		
		temp();
	}
	
	__asm("cps 0x10"); // CPU to USER mode

	while (1)
		;
}
