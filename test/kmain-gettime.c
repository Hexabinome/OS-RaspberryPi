#include "stdint.h"
#include "syscall.h"

void kmain( void )
{
    __asm("cps 0x10"); // switch CPU to USER mode
    // **********************************************************************

    uint64_t time = sys_gettime();

    time++; // suppress compiler error
    return;
}
