// jorge rios and tomas g.
#include "sched_ops.h"
#include "proc_ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s [-fifo | -rr | -spn] [-queue_size #] <process_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Scheduler *scheduler;
    char *process_file = NULL;
    int queue_size = 10; // Default size

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-queue_size") == 0) {
                if (i + 1 < argc) {
                    queue_size = atoi(argv[++i]);
                    if (queue_size <= 0) {
                        fprintf(stderr, "Queue size must be positive\n");
                        return EXIT_FAILURE;
                    }
                } else {
                    fprintf(stderr, "Missing queue size value\n");
                    return EXIT_FAILURE;
                }
            } else if (strcmp(argv[i], "-fifo") == 0 || strcmp(argv[i], "-rr") == 0 || strcmp(argv[i], "-spn") == 0) {
                scheduler = create_scheduler(argv[i]);
                scheduler->queue_size = queue_size;
                // Reinitialize semaphores with new queue size
                sem_destroy(&scheduler->empty_sem);
                sem_destroy(&scheduler->full_sem);
                sem_init(&scheduler->empty_sem, 0, queue_size);
                sem_init(&scheduler->full_sem, 0, 0);
            } else {
                fprintf(stderr, "Invalid option: %s\n", argv[i]);
                return EXIT_FAILURE;
            }
        } else {
            process_file = argv[i];
        }
    }

    if (!scheduler || !process_file) {
        fprintf(stderr, "Usage: %s [-fifo | -rr | -spn] [-queue_size #] <process_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Load process data
    if (scheduler->load_process_data(process_file, scheduler) != 0)
    {
        fprintf(stderr, "Error loading process data\n");
        scheduler->destroy_sched_queue(scheduler);
        return EXIT_FAILURE;
    }

    // Create scheduler threads
    pthread_t long_term_thread, short_term_thread, timer_thread;

    // Start global timer
    if (pthread_create(&timer_thread, NULL, timer_function, scheduler) != 0)
    {
        perror("Error creating timer thread");
        return EXIT_FAILURE;
    }

    // Start long-term scheduler
    if (pthread_create(&long_term_thread, NULL, long_term_scheduler, scheduler) != 0)
    {
        perror("Error creating long-term scheduler thread");
        return EXIT_FAILURE;
    }

    // Start short-term scheduler
    if (pthread_create(&short_term_thread, NULL, short_term_scheduler, scheduler) != 0)
    {
        perror("Error creating short-term scheduler thread");
        return EXIT_FAILURE;
    }

    // Wait for threads to complete
    pthread_detach(long_term_thread);
    pthread_detach(short_term_thread);
    pthread_join(timer_thread, NULL);

    // Clean up resources
    scheduler->destroy_sched_queue(scheduler);

    return EXIT_SUCCESS;
}