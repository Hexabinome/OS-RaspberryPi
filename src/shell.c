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
	else if (strcmp(cmd_name, "clear") == 0)
	{
		return &do_clear;
	}

	return NULL;
}

extern struct sem_s cmd_buffer_sem;
extern struct sem_s shell_sem;
extern char* cmd_buffer;

int start_shell()
{
	int argc;
	
	while (1)
	{
		fb_prompt();
		sem_up(&cmd_buffer_sem);
		
		sem_down(&shell_sem); // wait until shell is ready
		
		char** args = parse_command(cmd_buffer, &argc);

		command_t* command = find_command(args[0]);
		if (command == NULL)
		{
			fb_print_text("Command not found\n");
		}
		else
		{
			fb_print_text("BEFORE FORK\n");
			int pid = sys_fork();
			fb_print_text("AFTER FORK\n");
			if (pid == 0)
			{
				command(argc-1, args+1); // skip command name
				sys_exit(0);
			}
			else
			{
				int cmd_status;
				sys_waitpid(pid, &cmd_status);
				fb_print_char('\n');
				// TODO fill shell variable of last return code: $?
			}
		}
	}

	return 0;
}
