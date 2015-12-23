#include "sched.h"
#include "malloc.h"

void kmain()
{
	sched_init();

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


}
