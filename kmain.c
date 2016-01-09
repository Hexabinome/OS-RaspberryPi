#include "sched.h"
#include <stdint.h>
#include "util.h"
#include "hw.h"
#include "keyboard.h"
#include "syscall.h"

static uint32_t temp()
{
	uint32_t i;
	for (i = 0; i < 5000000; i++) ;
	
	return i;
}

void blink()
{
	led_off();
	temp();
	led_on();
	temp();
	led_off();
	temp();
	led_blink();
	temp();
	led_on();
}

static int keyboardProcess()
{
	temp();
	led_on();
	temp();
	led_off();
	temp();
	led_blink();
	temp();
	
	led_on();
	UsbInitialise();
	temp();
	led_off();
	temp();
	led_blink();
	temp();
	
	while (1)
	{
		led_on();
		KeyboardUpdate();
		temp();
		led_off();
		temp();
		
		char c = KeyboardGetChar();
		if (c == 'a')
		{
			c++;
			//led_on();
		}
		/*else
		{
			led_off();
		}*/
	}
	
	return 0;
}

void kmain()
{
	hw_init();
	sched_init();
	
	create_process(&keyboardProcess);
		
	sched_start();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	while (1)
	{
		sys_yield();
	}
}
