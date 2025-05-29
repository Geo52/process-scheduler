#ifndef SCHEDULER_OPS_H
#define SCHEDULER_OPS_H

#include "scheduler.h"

// The scheduler functions
void scheduler_init(Scheduler* scheduler);
void scheduler_destroy(Scheduler* scheduler);
int scheduler_load_data(const char* filename, Scheduler* scheduler);
void scheduler_create_process(Scheduler* scheduler, int pid, int arrival_time, int service_time);
PCB* scheduler_next_process_fifo(Scheduler* scheduler);
PCB* scheduler_next_process_rr(Scheduler* scheduler);
PCB* scheduler_next_process_spn(Scheduler* scheduler); // Added SPN scheduler
void scheduler_signal_worker(PCB *process);
void scheduler_context_switch(Scheduler* scheduler, PCB* process);
void scheduler_wait_for_cpu(Scheduler* scheduler);
void scheduler_release_cpu(Scheduler* scheduler);
void scheduler_wait_for_worker(Scheduler* scheduler);
void scheduler_wait_for_queue(Scheduler* scheduler);
int scheduler_get_time(Scheduler* scheduler);

#endif // SCHEDULER_OPS_H