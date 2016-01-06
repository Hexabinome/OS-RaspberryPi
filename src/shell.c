#include "shell.h"
#include "syscall.h"
//#include "fb_cursor.h"
#include "string.h"
#include "malloc.h"
#include "shell_commands.h"

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
	else if (strcmp(args[0], "ps") == 0)
	{
		do_ps();
	}
}


int start_shell()
{
	//fb_prompt();
	int i = 0;
	// Read line
	char* m = "ps";
	
	// Call corresponding command
	int pid = sys_fork();
	if (pid == 0)
	{
		launch_command(m);
		sys_exit(0);
	}
	else
	{
		int cmd_status;
		sys_waitpid(pid, &cmd_status);
	}
	sys_exit(0);
	return 0;
}

