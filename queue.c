#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

// Initialize a new queue
void init_queue(ReadyQueue* queue) {
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    pthread_mutex_init(&queue->mutex, NULL);
}

// Clean up queue resources
void destroy_queue(ReadyQueue* queue) {
    // Free all remaining processes if needed
    PCB* current = queue->head;
    PCB* next;
    
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    
    queue->head = NULL;
    queue->tail = NULL;
}

// Add a process to the queue (at the end)
void enqueue(ReadyQueue* queue, PCB* process) {
    process->next = NULL;
    
    if (queue->tail == NULL) { // Empty queue
        queue->head = process;
        queue->tail = process;
    } else {
        queue->tail->next = process;
        queue->tail = process;
    }
    
    queue->size++;
}

// Remove and return the first process from the queue
PCB* dequeue(ReadyQueue* queue) {
    PCB* process = NULL;
    
    if (queue->head != NULL) {
        process = queue->head;
        queue->head = process->next;
        
        if (queue->head == NULL) {
            queue->tail = NULL;
        }
        
        process->next = NULL;
        queue->size--;
    }
    
    return process;
}

// Look at the first process without removing it
PCB* peek(ReadyQueue* queue) {
    PCB* process = NULL;

    process = queue->head;
    
    return process;
}

// Check if queue is empty
int is_empty(ReadyQueue* queue) {
    int empty;

    empty = (queue->size == 0);
    
    return empty;
} 