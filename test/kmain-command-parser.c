#include "kernel.h"
#include "shell.h"

void kmain()
{
	init_kernel();

	char* cmd = "hello    my name  is         \"david wobrock\"\n";
	int argc;
	char** res = parse_command(cmd, &argc);
	res++;
	return;
}

