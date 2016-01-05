#include "shell.h"
#include "syscall.h"
#include "fb_cursor.h"
#include "string.h"
#include "hw.h"
#include "malloc.h"

void do_echo(char* args)
{
	fb_print_text(args);
}

static char** parse_command(char* cmd)
{
	char* space = strtok(cmd, ' ');
	space++;
	
	char** args = (char**) gmalloc(sizeof(char*) * 2);
	args[0] = cmd;
	args[1] = space;
	
	return args;
}

void launch_command(char* cmd)
{
	char** args = parse_command(cmd);
	if (strcmp(args[0], "echo") == 0)
	{
		do_echo(args[1]);
	}
	fb_print_char('\n');
}


int start_shell()
{
	// while(1)
	fb_prompt();
	
	// Read line
	char* m = "echo Hello world";
	fb_print_text(m);
	
	// Call corresponding command
	int pid = sys_fork();
	if (pid == 0)
	{
		launch_command(m);
	}
	else
	{
		int cmd_status;
		sys_waitpid(pid, &cmd_status);
	}
	
	sys_exit(0);
	return 0;
}

