#include "sem.h"
#include "syscall.h"
#include "pwm.h"
#include "hw.h"

#define N 100

struct sem_s mutex, vide, plein;

static unsigned int obj = 0;
static unsigned int counter = 0;
static unsigned int chain[N];

static void produire_objet(unsigned int* i)
{
	*i = obj++;
	log_str("Produce\n");
	for (int a = 0; a < 100000; ++a) ;
}

static void mettre_objet(unsigned int* i)
{
	chain[counter++] = *i;
	log_str("Put in chain\n");
}

static void retirer_object(unsigned int* i)
{
	*i = chain[counter--];
	log_str("Take from chain\n");
}

static void utiliser_objet(unsigned int* i)
{
	*i = -1;
	log_str("Use\n");
	for (int a = 0; a < 100000; ++a) ;
}

int producteur()
{
	unsigned int i;
	while (1)
	{
		produire_objet(&i);
		sem_down(&vide);
		
		sem_down(&mutex);
		mettre_objet(&i);
		sem_up(&mutex);
		
		sem_up(&plein);
	}
	
	return 0;
}


int consommateur()
{
	unsigned i;
	while (1)
	{
		sem_down(&plein);
		
		sem_down(&mutex);
		retirer_object(&i);
		sem_up(&mutex);
		
		sem_up(&vide);
		utiliser_objet(&i);
	}
}

void kmain()
{
	audio_init();
	sched_init();
	
	create_process(&producteur);
	create_process(&consommateur);
	
	irq_init();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	sem_init(&mutex, 1);
	sem_init(&vide, N);
	sem_init(&plein, 0);
	
	while (1)
	{
		sys_yield();
	}
}
