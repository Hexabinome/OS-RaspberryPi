#ifndef SCHED_SVC_H
#define SCHED_SVC_H

#include <stdint.h>

void context_load_from_pcb_svc(uint32_t* sp);
void context_save_to_pcb_svc(uint32_t* sp);

#endif // SCHED_SVC_H
