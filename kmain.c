#include "sched.h"
#include "shell.h"
#include "kernel.h"
#include "syscall.h"
#include "fb_cursor.h"
#include "hw.h"
#include "keyboard.h"
#include "malloc.h"
#include "sem.h"

struct sem_s cmd_buffer_sem;
struct sem_s shell_sem;
char* cmd_buffer;
uint32_t cmd_buffer_idx;

#include "vmem.h"
#include "sched.h"
#include "process.h"
extern struct pcb_s* current_process;
static int keyboard()
{
	cmd_buffer = (char*) gmalloc(sizeof(char) * 100);
	cmd_buffer_idx = 0;
	
	fb_print_int((uint32_t) &cmd_buffer);
	fb_print_char('\n');
	fb_print_int((uint32_t) &current_process);
	fb_print_char('\n');
	fb_print_int((uint32_t) vmem_translate((uint32_t) &cmd_buffer, current_process));
	fb_print_char('\n');
	
	sem_down(&cmd_buffer_sem); // begin when shell says okay
	
	char c;
	while (1)
	{
		KeyboardUpdate();
		c = KeyboardGetChar();
		
		if (c != NULL)
		{
			// Display
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


			cmd_buffer[cmd_buffer_idx++] = c;
			cmd_buffer[cmd_buffer_idx] = '\0';
			if (c == '\n')
			{
				sem_up(&shell_sem); // unblock shell
				sem_down(&cmd_buffer_sem); // block command line
				fb_print_text("CONTINUE CMD\n");
				cmd_buffer_idx = 0;
				cmd_buffer[cmd_buffer_idx] = '\0';
			}
		}
	}
	
	return 0;
}

void kmain()
{
	init_kernel();
	
	create_process(&keyboard);
	create_process(&start_shell);
	
	sem_init(&cmd_buffer_sem, 0);
	sem_init(&shell_sem, 0);
	
	start_kernel();
	

	while (1)
	{
		sys_yield();
	}
}
