#include "shell.h"
#include "syscall.h"
#include "fb_cursor.h"
#include "string.h"
#include "malloc.h"
#include "shell_commands.h"
#include "config.h"
#include "command_parser.h"
#include "sem.h"


typedef void (command_t) (int, char**);


static command_t* find_command(char* cmd_name)
{
	if (strcmp(cmd_name, "echo") == 0)
	{
		return &do_echo;
	}
	else if (strcmp(cmd_name, "ps") == 0)
	{
		return &do_ps;
	}
	else if (strcmp(cmd_name, "fork") == 0)
	{
		return &do_fork;
	}
	else if (strcmp(cmd_name, "music") == 0)
	{
		return &do_music;
	}

	return NULL;
}

extern struct sem_s cmd_buffer_sem;
extern struct sem_s shell_sem;
extern char* cmd_buffer;

#include "hw.h"
#include "vmem.h"
#include "sched.h"
#include "process.h"
extern struct pcb_s* current_process;
int start_shell()
{
	int argc;
	
	while (1)
	{
		fb_prompt();
		sem_up(&cmd_buffer_sem);
		
		sem_down(&shell_sem); // wait until shell is ready
		
		fb_print_int((uint32_t) &cmd_buffer);
		fb_print_char('\n');
		fb_print_int((uint32_t) &current_process);
		fb_print_char('\n');
		fb_print_int((uint32_t) vmem_translate((uint32_t) &cmd_buffer, current_process));
		fb_print_char('\n');
		fb_print_text(cmd_buffer);
		
		char** args = parse_command(cmd_buffer, &argc);
		fb_print_text("GO3 SHELL\n");
		fb_print_text(cmd_buffer); // TO DELETE

		command_t* command = find_command(args[0]);
		if (command == NULL)
		{
			fb_print_text("Command not found\n");
		}
		else
		{
			int pid = sys_fork();
			if (pid == 0)
			{
				command(argc-1, args+1); // skip command name
				sys_exit(0);
			}
			else
			{
				int cmd_status;
				sys_waitpid(pid, &cmd_status);
				// TODO fill shell variable of last return code: $?
			}
		}
	}

	return 0;
}
