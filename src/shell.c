#include "shell.h"
#include "syscall.h"
//#include "fb_cursor.h"

void launch_command(char* cmd)
{
	uint32_t i;
	for (i = 0; i < 1000000000; ++i) ;
}


int start_shell()
{
	//fb_prompt();
	
	// Read line
	char* m = "ps\n";
	
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

