#include "process.h"
#include "scheduler.h"
#include "proc_ops.h"
#include "sched_ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// External reference to the global scheduler
extern Scheduler scheduler;

// Create a new process
PCB* create_process(int pid, int arrival_time, int service_time, Scheduler *scheduler) {
    PCB* process = (PCB*)malloc(sizeof(PCB));
    if (process == NULL) {
        perror("Failed to allocate memory for process");
        return NULL;
    }
    
    // Initialize process attributes
    process->pid = pid;
    process->arrival_time = arrival_time;
    process->service_time = service_time;
    process->remaining_time = service_time;
    process->completion_time = 0;
    process->turnaround_time = 0;
    process->waiting_time = 0;
    process->scheduler = scheduler;
    
    process->state = PROCESS_NEW;
    process->next = NULL;
    
    // Initialize function pointers
    process->wait_for_cpu = process_wait_for_cpu;
    process->release_cpu = process_release_cpu;
    process->cleanup = process_cleanup;
    process->enter_ready_queue = process_enter_ready_queue;
    
    // Initialize process semaphore (initially blocked)
    if (sem_init(&process->process_sem, 0, 0) != 0) {
        perror("Failed to initialize process semaphore");
        free(process);
        return NULL;
    }
    
    return process;
}

// Implementation of the process execution
void* process_execute(void* arg) {
    PCB* process = (PCB*)arg;
    
    // Update process state to READY and add to ready queue
    process->state = PROCESS_READY;
    printf("Process %d enters ready state at time %d\n", 
           process->pid, process->scheduler->current_time);
    
    // Add process to the ready queue
    process->enter_ready_queue(process->scheduler, process);
    
    while (process->remaining_time > 0) {
        // Wait for scheduler to give CPU time
        process->wait_for_cpu(process);

        // Process is now running
        printf("Process %d is now running at time %d with %d time remaining\n", 
               process->pid, process->scheduler->current_time, process->remaining_time);
        
        // For RR, run for time quantum or remaining time, whichever is smaller
        int run_time = process->remaining_time;
        if (process->scheduler->next_process == scheduler_next_process_rr) {
            run_time = (process->remaining_time < process->scheduler->time_quantum) ? 
                       process->remaining_time : process->scheduler->time_quantum;
        }
        
        // Execute for specified time units
        while (run_time > 0) {
            sleep(1);
            pthread_mutex_lock(&process->scheduler->timer_mutex);
            process->remaining_time--;
            pthread_mutex_unlock(&process->scheduler->timer_mutex);
            run_time--;
        }
        
        // If process not done and using RR, go back to ready queue
        if (process->remaining_time > 0 && process->scheduler->next_process == scheduler_next_process_rr) {
            printf("Process %d preempted at time %d with %d time remaining\n",
                   process->pid, process->scheduler->current_time, process->remaining_time);
            process->state = PROCESS_READY;
            process->release_cpu(process);
            process->enter_ready_queue(process->scheduler, process);
        }
    }
    
    // Process has completed
    process->state = PROCESS_TERMINATED;
    
    // Calculate statistics
    pthread_mutex_lock(&process->scheduler->timer_mutex);
    process->completion_time = process->scheduler->current_time;
    pthread_mutex_unlock(&process->scheduler->timer_mutex);
    
    process->turnaround_time = process->completion_time - process->arrival_time;
    process->waiting_time = process->turnaround_time - process->service_time;
    
    printf("Process %d completed at time %d (turnaround: %d, waiting: %d)\n", 
           process->pid, process->completion_time, 
           process->turnaround_time, process->waiting_time);
    
    // Mark CPU as idle
    pthread_mutex_lock(&process->scheduler->cpu_mutex);
    process->scheduler->cpu_idle = 1;
    pthread_mutex_unlock(&process->scheduler->cpu_mutex);
    
    // Increment completed process count
    process->scheduler->completed_count++;
    
    // Release CPU
    process->release_cpu(process);
    
    // Cleanup process resources
    if (process->cleanup) {
        process->cleanup(process);
    }
    
    pthread_exit(0);
}