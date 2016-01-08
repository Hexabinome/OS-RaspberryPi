#include "util.h"
#include "hw.h"

void
kernel_panic(char* string, int number)
{
    for(;;)
    {
        /* do nothing */
    }
}

int pow(int x, int y)
{
	if(y == 0){
		return 1;
	}
	int powret = 1;
	while(y > 0)
	{
		y--;
		powret = powret*x;
	}
	return powret;	
}

int mod(int a, int b)
{
	int c = divide(a,b);
	return (a - (b * c));
}
