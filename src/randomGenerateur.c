#include "randomGenerateur.h"


int comteur = 1;
short x = 1;
short a = 31541;
short c = 1;

int getRandomNb()
{
	x = a*x + c;
	
	int result = (int) x;

	// Sachant que le modulo coute cher, on utilise 
	//		un masque pour réduire le nombre aléatoire
	return (result & 127);
}

