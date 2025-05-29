#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"
#include "queue.h"
#include <pthread.h>
#include <semaphore.h>

// Process data structure for storing arrival and service times
typedef struct {
    int arrival_time;
    int service_time;
} ProcessData;

// Global scheduler variables
typedef struct Scheduler {
    int current_time;             // Global timer
    pthread_mutex_t timer_mutex;  // Mutex for timer access
    
    int cpu_idle;                 // Flag indicating if CPU is idle
    pthread_mutex_t cpu_mutex;    // Mutex for CPU access
    sem_t cpu_sem;                // Semaphore for CPU allocation
    PCB* current_running_process; // Pointer to the currently running process
    
    ReadyQueue* ready_queue;      // Queue of ready processes
    int queue_size;               // Maximum size of ready queue
    sem_t empty_sem;              // Semaphore for empty slots in queue
    sem_t full_sem;               // Semaphore for full slots in queue
    int process_count;            // Total number of processes
    int completed_count;          // Number of completed processes
    
    // Process data storage
    ProcessData* process_data;    // Array of process data
    int num_processes_to_create;  // Number of processes to create
    
    int is_running;               // Flag to control simulation
    
    // Scheduling algorithm parameters
    int time_quantum;             // For Round Robin
    
    // Function pointers for scheduler operations
    void (*init_sched_queue)(struct Scheduler* scheduler);
    void (*destroy_sched_queue)(struct Scheduler* scheduler);
    int (*load_process_data)(const char* filename, struct Scheduler* scheduler);
    void (*create_process)(struct Scheduler* scheduler, int pid, int arrival_time, int service_time);
    PCB* (*next_process)(struct Scheduler* scheduler);
    void (*context_switch)(struct Scheduler* scheduler, struct PCB* process);
    void (*wait_for_worker)(struct Scheduler* scheduler);
    void (*signal_worker)(struct PCB* process);
    void (*wait_for_cpu)(struct Scheduler* scheduler);
    void (*release_cpu)(struct Scheduler* scheduler);
    void (*wait_for_queue)(struct Scheduler* scheduler);
    int (*get_time)(struct Scheduler* scheduler);
} Scheduler;

// Create and initialize a new scheduler
Scheduler* create_scheduler(char* sched_algorithm);

// Functions for the scheduler threads
void* long_term_scheduler(void* arg);
void* short_term_scheduler(void* arg);
void* timer_function(void* arg);

#endif // SCHEDULER_H