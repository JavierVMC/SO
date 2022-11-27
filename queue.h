// C program for array implementation of queue
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// A structure to represent a queue
typedef struct Queue
{
    pthread_t front, rear, size;
    unsigned capacity;
    pthread_t *array;
} Queue;

// function to create a queue
// of given capacity.
// It initializes size of queue as 0
Queue *createQueue(unsigned capacity);

// Queue is full when size becomes
// equal to the capacity
pthread_t isFull(Queue *queue);

// Queue is empty when size is 0
pthread_t isEmpty(Queue *queue);

// Function to add an item to the queue.
// It changes rear and size
void enqueue(Queue *queue, pthread_t item);

// Function to remove an item from queue.
// It changes front and size
pthread_t dequeue(Queue *queue);

// Function to get front of queue
pthread_t front(Queue *queue);

// Function to get rear of queue
pthread_t rear(Queue *queue);