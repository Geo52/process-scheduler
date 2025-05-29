#ifndef PROCESS_H
#define PROCESS_H

#include <pthread.h>
#include <semaphore.h>

// Forward declarations
struct PCB;
struct Scheduler;
struct ReadyQueue;

// Process states
typedef enum {
    PROCESS_NEW,
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_WAITING,
    PROCESS_TERMINATED
} ProcessState;

// Process Control Block (PCB)
typedef struct PCB {
    int pid;                    // Process ID
    int arrival_time;           // Time when process should enter system
    int service_time;           // CPU burst time required
    int remaining_time;         // Remaining execution time
    int completion_time;        // Time when process completes
    int turnaround_time;        // Time in system (completion - arrival)
    int waiting_time;           // Time spent waiting for CPU
    
    ProcessState state;         // Current state of the process
    pthread_t thread;           // Thread ID of the process
    sem_t process_sem;          // Semaphore to control process execution
    
    // Function pointers for process operations, the names below should be the only ones you use during process execution
    void (*enter_ready_queue)(struct Scheduler* scheduler, struct PCB* process);    // Function to enter the ready queue for the first time
    void (*wait_for_cpu)(struct PCB* process);                                      // Function to wait for the scheduler to give CPU
    void (*release_cpu) (struct PCB* process);                                      // Function to exit the CPU
    void (*cleanup) (struct PCB* process);                                          // Function to clean up resources

    struct Scheduler* scheduler;    // Pointer to the scheduler
    struct PCB* next;               // Pointer to next process in queue
} PCB;

// Function to create a new process
PCB* create_process(int pid, int arrival_time, int service_time, struct Scheduler *scheduler); 

// Function for the process vCPU
void* process_execute(void* arg);

#endif // PROCESS_H 