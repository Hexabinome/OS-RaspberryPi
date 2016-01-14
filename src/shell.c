#include "shell.h"
#include "syscall.h"
#include "fb_cursor.h"
#include "string.h"
#include "malloc.h"
#include "shell_commands.h"
#include "config.h"
#include "command_parser.h"


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


int start_shell()
{
	int argc;
	
	//while (1)
	fb_prompt();

	// Read line
	char* cmd_line = "fork\n";
	//char* cmd_line = "ps\n";
	//char * useless_str = "Hello world\n";
	//fb_print_text(useless_str);
	//char* cmd_line = "clear\n";
	fb_print_text(cmd_line);
	
	char** args = parse_command(cmd_line, &argc);

	command_t* command = find_command(args[0]);
	if (command == NULL)
	{
		fb_print_text("Command not found\n");
	}
	else
	{
		/*int pid = sys_fork();
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
		}*/
		command(argc-1, args+1);
	}

	return 0;
}
