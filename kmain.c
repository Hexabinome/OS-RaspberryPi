#include "usbd/usbd.h"
#include "device/hid/keyboard.h"
#include "sched.h"
#include <stdint.h>
#include "util.h"
#include "hw.h"

static int keyboardProcess()
{
	while (1)
	{
		UsbCheckForChange();
		uint32_t a = KeyboardCount();
		switch (a)
		{
			case 0:
				break;
			case 1:
				log_str("1\n");
				break;
			case 2:
				log_str("2\n");
				break;
			default:
				PANIC();
		}
	}
	
	return 0;
}

void kmain()
{
	sched_init();
	
	UsbInitialise();
	create_process(&keyboardProcess);
		
	sched_start();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	while (1) ;
}
