#include "sched.h"
#include "malloc.h"
#include "syscall.h"

static int process1()
{

    int* t1 = (int*)gmalloc(sizeof(int));
    int* t2 = (int*)gmalloc(sizeof(int));
    int diff = ((int) t1 - (int) t2) == (HEAP_BLOCK_SIZE + sizeof(int));
    //diff should be HEAP_BLOCK_SIZE+sizeof(int)

    //test if block is correctly freed
    int* test1 = (int*)gmalloc(sizeof(int));
    gfree(test1);
    int* test2 = (int*)gmalloc(sizeof(int));
    gfree(test2);
    // test1 and test2 should have same value

    // test if blocks are correctly merged
    int* a = (int*)gmalloc(sizeof(int));
    int* b = (int*)gmalloc(sizeof(int));
    int* c = (int*)gmalloc(sizeof(int));

    gfree(a);
    gfree(b);
    gfree(c);

    int* aa = (int*)gmalloc(sizeof(int)*2);
    gfree(aa);
    //aa and a should have the same value
    diff++;
    
    // The heap pointer of the current process should point on a heap_block 
    
    return 0;
}

void kmain()
{
    sched_init();
    create_process(&process1);
    irq_init();
    sys_yield();
}
