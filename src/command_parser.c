#include "command_parser.h"
#include "string.h"
#include <stdint.h>
#include "malloc.h"

static void clean_command(char * cmd)
{
	int i = 0;
	while (cmd[i] != '\n')
	{
		i++;
	}
	cmd[i] = '\0';

}

char** parse_command(char* cmd, int* argc)
{
	clean_command(cmd);
	unsigned int len = strlen(cmd);
	
	char* space = strtok(cmd, ' ');
	char** args = (char**) gmalloc(sizeof(char*));
	args[0] = cmd;
	
	uint32_t counter = 1;
	char* end_pos = &(cmd[len-1]);
	char* next_space;
	while (space <= end_pos)
	{
		next_space = strtok(cmd, ' ');
		
		if (space+1 != next_space) // the two spaces are not one after another, so a word has been found
		{
			args = (char**) grealloc(args, sizeof(char*) * (counter++));
			args[counter-1] = space+1;
		}
		
		space = next_space;
	}
	
	*argc = counter;

	return args;
}
