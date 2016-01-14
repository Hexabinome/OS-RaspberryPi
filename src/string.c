#include "string.h"
#include "config.h"
#include <stdint.h>

static char* strfnd_replace(char* str, const char delim, uint8_t replace)
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
	
	if (replace)
	{
		*current = '\0';
	}

	return current;
}

char* strtok(char* str, const char delim)
{
	return strfnd_replace(str, delim, TRUE);
}

char* strfnd(char* str, const char delim)
{
	return strfnd_replace(str, delim, FALSE);
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

// http://www.opensource.apple.com/source/Libc/Libc-167/gen.subproj/i386.subproj/strlen.c
unsigned int strlen(const char* str)
{
	register const char* s;
	
	for (s = str; *s; ++s) ;
	
	return (s - str);
}
