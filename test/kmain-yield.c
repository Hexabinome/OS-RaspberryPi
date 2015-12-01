#include "syscall.h"
#include "sched.h"

#define NB_PROCESS 5

void user_process()
{
    int v=0;
    for(;;)
    {
        v++;
        sys_yield();
    }
}

void kmain( void )
{
    sched_init();

    int i;
    for(i=0;i<NB_PROCESS;i++)
    {
        create_process((func_t*)&user_process);
    }

    __asm("cps 0x10"); // switch CPU to USER mode
    // ******************************************

    while(1)
    {
        sys_yield();
    }
}
