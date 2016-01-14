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


static char* all_cmd[] = 
{
	"ps\n",
	"echo hi\n",
	"music 0 2 1\n"
};
static const uint8_t arg_nb = 3;

static void temp()
{
	uint32_t i = 0;
	while (i++ < 20000000)
		;
}

int start_shell()
{
	int argc;
	
	fb_print_text(all_cmd[0]);
	fb_print_text(all_cmd[1]);
	fb_print_text(all_cmd[2]);
	
	uint8_t i;
	for (i = 0; i < arg_nb; ++i)
	{
		fb_prompt();
		char* cmd = all_cmd[i];
		fb_print_text(cmd);
		temp();
		
		fb_print_text("Loading command... ");
		char** args = parse_command(cmd, &argc);
		fb_print_text("Ok. ");
		command_t* command = find_command(args[0]);
		fb_print_text("Done.\n");
		
		if (command == NULL)
		{
			fb_print_text("Command not found\n");
		}
		else
		{
			/*fb_print_text("BEFORE FORK\n");
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
			}*/
			command(argc-1, args+1);
			fb_print_char('\n');
		}
		temp();
	}

	return 0;
}
