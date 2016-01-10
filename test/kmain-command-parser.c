#include "kernel.h"
#include "command_parser.h"

void kmain()
{
	init_kernel();

	char* cmd = "hello    my name  is         \"david wobrock\"     yo   \n";
	int argc;
	char** res = parse_command(cmd, &argc);
	
	cmd = "echo";
	res = parse_command(cmd, &argc);
	
	cmd = "echo \"Hello world!\"";
	res = parse_command(cmd, &argc);
	
	res++;
	return;
}

