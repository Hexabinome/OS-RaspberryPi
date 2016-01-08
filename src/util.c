#include "util.h"
#include "hw.h"

static const int TEN_POW[10] = {	1,
									10,
									100,
									1000,
									10000,
									100000,
							        1000000,
							        10000000,
							        100000000,
							        1000000000 };
							        
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

void int_to_str(char* str, int n)
{
	int ten_pow;
	int str_i = 0;
	int start_conv = 0;

	// Cas du 0
	if (n == 0)
	{
		str[0] = '0';
		str[1] = 0;
		return;
	}

	// Négatif ?
	if ((n & 0x80000000) != 0)
	{
		str[str_i++] = '-';
		n -= 1;
		n = ~n;
	}

	for (ten_pow = 9; ten_pow >= 0; ten_pow--)
	{
		int digit = 0;

		while (n >= TEN_POW[ten_pow])
		{
			n -= TEN_POW[ten_pow];
			digit++;
		}
		if (digit > 0)
		{
			str[str_i++] = (char)(digit + 48);

			if (start_conv == 0)
			{
				int zero_pad_i;

				for (zero_pad_i = 0; zero_pad_i < ten_pow; zero_pad_i++)
				{
					str[str_i + zero_pad_i] = '0';
				}
				str[str_i + zero_pad_i] = 0;
				
				start_conv = 1;
			}
		}
		else if (start_conv == 1)
		{
			str_i++;
		}
	}

}
