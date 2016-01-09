#include "string.h"
#include "config.h"

char* strtok(char* str, const char delim)
{
	if (str == NULL)
	{
		return NULL;
	}
	
	char* current = str;
	while (*current != delim)
	{
		current++;
	}
	
	*current = '\0';
	return current;
}

// http://www.jbox.dk/sanos/source/lib/string.c.html
int strcmp(const char* s1, const char* s2)
{
	int ret = 0;
	while (!(ret = *(unsigned char*) s1 - *(unsigned char*) s2) && *s2) ++s1, ++s2;
	
	if (ret < 0)
	{
		ret = -1;
	}
	else if (ret > 0)
	{
		ret = 1;
	}
	
	return ret;
}
