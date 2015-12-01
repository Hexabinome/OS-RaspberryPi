#ifndef __SYSCALL__
#define __SYSCALL__

#include <stdint.h>
#include "sched.h"

void sys_reboot();
void sys_nop();
void sys_settime(uint64_t date_ms);
uint64_t sys_gettime();
void sys_yieldto(struct pcb_s* dest);
void sys_yield();
void sys_exit(int status);

#endif // __SYSCALL__
