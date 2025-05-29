#include "sched_ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>

// Create and initialize a new scheduler
Scheduler *create_scheduler(char *sched_algorithm)
{
    Scheduler *scheduler = (Scheduler *)malloc(sizeof(Scheduler));
    if (scheduler == NULL)
    {
        perror("Failed to allocate memory for scheduler");
        return NULL;
    }

    // Initialize function pointers
    scheduler->init_sched_queue = scheduler_init;
    scheduler->destroy_sched_queue = scheduler_destroy;
    scheduler->load_process_data = scheduler_load_data;
    scheduler->create_process = scheduler_create_process;
    scheduler->wait_for_cpu = scheduler_wait_for_cpu;
    scheduler->wait_for_worker = scheduler_wait_for_worker;
    scheduler->wait_for_queue = scheduler_wait_for_queue;
    scheduler->signal_worker = scheduler_signal_worker;
    scheduler->release_cpu = scheduler_release_cpu;
    scheduler->get_time = scheduler_get_time;
    scheduler->context_switch = scheduler_context_switch;

    // Initialize scheduler algorithm
    if (!strcmp(sched_algorithm, "-fifo"))
        scheduler->next_process = scheduler_next_process_fifo;
    else if (!strcmp(sched_algorithm, "-rr"))
        scheduler->next_process = scheduler_next_process_rr;
    else if (!strcmp(sched_algorithm, "-spn"))
        scheduler->next_process = scheduler_next_process_spn;
    else
        exit(EXIT_FAILURE);

    // Initialize the scheduler
    scheduler->init_sched_queue(scheduler);

    return scheduler;
}

// implementation of long-term scheduler
void *long_term_scheduler(void *arg)
{
    Scheduler *scheduler = (Scheduler *)arg;
    int next_process = 0;
    int pid = 1;

    // Create processes at their arrival times
    while (scheduler->is_running && next_process < scheduler->num_processes_to_create)
    {
        int current_time;

        // get current time
        current_time = scheduler->get_time(scheduler);

        // Check if it's time to create the next process
        if (current_time >= scheduler->process_data[next_process].arrival_time)
        {
            if (scheduler->create_process)
            {
                scheduler->create_process(scheduler, pid, scheduler->process_data[next_process].arrival_time, scheduler->process_data[next_process].service_time);
            }
            pid++;
            next_process++;
        }

        // Sleep a bit to avoid busy waiting
        usleep(10000); // 10ms
    }

    printf("Long-term scheduler finished creating all processes\n");

    // Free process data after all processes are created
    if (scheduler->process_data)
    {
        free(scheduler->process_data);
        scheduler->process_data = NULL;
    }

    pthread_exit(0);
}

// implementation of short-term scheduler
void *short_term_scheduler(void *arg)
{
    Scheduler *scheduler = (Scheduler *)arg;

    while (scheduler->is_running)
    {
        // Check if the other scheduler is not using the CPU
        scheduler->wait_for_cpu(scheduler);

        if (!is_empty(scheduler->ready_queue) || scheduler->current_running_process != NULL)
        {
            // Get the next process
            PCB *process = scheduler->next_process(scheduler);

            // Process switch
            scheduler->context_switch(scheduler, process);

            if (process != NULL)
            {

                printf("Time %d: Scheduling process %d to run (remaining time: %d)\n",
                       scheduler->current_time, process->pid, process->remaining_time);

                // Signal the process to start executing
                scheduler->signal_worker(process);

                // Wait for the process to terminate (non-preemptive)
                scheduler->wait_for_worker(scheduler);
            }
        }

        // release CPU
        scheduler->release_cpu(scheduler);

        // Sleep a bit to avoid busy waiting
        usleep(10000); // 10ms
    }

    pthread_exit(0);
}

// implementation of timer function
void *timer_function(void *arg)
{
    Scheduler *scheduler = (Scheduler *)arg;

    while (scheduler->is_running)
    {
        // Sleep for one time unit
        sleep(1);

        // Update global time
        pthread_mutex_lock(&scheduler->timer_mutex);
        scheduler->current_time++;
        pthread_mutex_unlock(&scheduler->timer_mutex);

        int current_time = scheduler->get_time(scheduler);

        // Print current time and current running process info
        pthread_mutex_lock(&scheduler->cpu_mutex);
        if (scheduler->current_running_process)
        {
            printf("Time: %d - Running Process: %d (remaining: %d)\n",
                   current_time,
                   scheduler->current_running_process->pid,
                   scheduler->current_running_process->remaining_time);
        }
        else
        {
            printf("Time: %d - CPU idle\n", current_time);
        }
        pthread_mutex_unlock(&scheduler->cpu_mutex);

        // Check if simulation is complete
        if (scheduler->completed_count >= scheduler->process_count && scheduler->process_count > 0)
        {
            scheduler->is_running = 0;
            printf("All processes completed. Ending simulation.\n");
        }
    }

    pthread_exit(0);
}
