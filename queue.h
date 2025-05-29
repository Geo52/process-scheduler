#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"
#include <pthread.h>

// Ready Queue structure
typedef struct {
    PCB* head;                  // Head of the linked list
    PCB* tail;                  // Tail of the linked list
    int size;                   // Number of processes in queue
    pthread_mutex_t mutex;      // Mutex for queue access
} ReadyQueue;

// Queue operations
void init_queue(ReadyQueue* queue);
void destroy_queue(ReadyQueue* queue);
void enqueue(ReadyQueue* queue, PCB* process);
PCB* dequeue(ReadyQueue* queue);
PCB* peek(ReadyQueue* queue);
int is_empty(ReadyQueue* queue);

#endif // QUEUE_H 