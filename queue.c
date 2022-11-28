// C program for array implementation of queue
#include "queue.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// function to create a queue
// of given capacity.
// It initializes size of queue as 0
Queue createQueue()
{
    Queue queue;
    queue.front = 0;
    queue.size = 0;

    // This is important, see the enqueue
    queue.rear = MAX_QUEUE_CAPACITY - 1;
    for(int i = 0; i < MAX_QUEUE_CAPACITY; i++){
        queue.array[i] = 1;
    }
    return queue;
}

// Queue is full when size becomes
// equal to the capacity
pthread_t isFull(Queue *queue)
{
    return (queue->size == MAX_QUEUE_CAPACITY);
}

// Queue is empty when size is 0
pthread_t isEmpty(Queue *queue)
{
    return (queue->size == 0);
}

// Function to add an item to the queue.
// It changes rear and size
void enqueue(Queue *queue, pthread_t item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % MAX_QUEUE_CAPACITY;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}

// Function to remove an item from queue.
// It changes front and size
pthread_t dequeue(Queue *queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    pthread_t item = queue->array[queue->front];
    queue->front = (queue->front + 1) % MAX_QUEUE_CAPACITY;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
pthread_t front(Queue *queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}

// Function to get rear of queue
pthread_t rear(Queue *queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}

// int main(){
//     Queue cola = createQueue();
//     enqueue(&cola, 10);
//     printf("Size: %ld\n", cola.size);
//     printf("Front: %ld\n", cola.front);
//     printf("Rear: %ld\n", cola.rear);
//     while(cola.size != 0){
//         printf("[%ld] dequeue\n", dequeue(&cola));
//         printf("Size: %ld\n", cola.size);
//         printf("Front: %ld\n", cola.front);
//         printf("Rear: %ld\n", cola.rear);
//     }

// }