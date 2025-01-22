#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <threads.h>

// Node structure representing a single element in the queue
typedef struct QueueNode
{
    void *data;
    struct QueueNode *next;
} QueueNode;

// Queue structure for managing the head, tail, and synchronization
typedef struct
{
    QueueNode *head;
    QueueNode *tail;
    size_t visited;
    mtx_t mutex;
    cnd_t notEmpty;
} Queue;

// Global queue instance
static Queue *queue = NULL;

// Initializes the queue and its synchronization primitives
void initQueue()
{
    queue = (Queue *)malloc(sizeof(Queue));
    if (queue == NULL)
    {
        perror("Memory allocation error");
        return;
    }

    queue->head = NULL;
    queue->tail = NULL;
    queue->visited = 0;

    // Initialize the mutex and condition variable
    if (mtx_init(&queue->mutex, mtx_plain) != thrd_success)
    {
        perror("Mutex initialization error");
        free(queue);
        queue = NULL;
        return;
    }
    if (cnd_init(&queue->notEmpty) != thrd_success)
    {
        perror("Condition variable initialization error");
        mtx_destroy(&queue->mutex);
        free(queue);
        queue = NULL;
        return;
    }
}

// Cleans up the queue and frees all resources
void destroyQueue()
{
    if (queue == NULL)
        return;

    mtx_lock(&queue->mutex);

    // Free all remaining nodes in the queue
    while (queue->head != NULL)
    {
        QueueNode *temp = queue->head;
        queue->head = queue->head->next;
        free(temp);
    }

    mtx_unlock(&queue->mutex);

    // Destroy the mutex and condition variable
    mtx_destroy(&queue->mutex);
    cnd_destroy(&queue->notEmpty);

    // Free the queue structure itself
    free(queue);
    queue = NULL;
}

// Adds a new item to the end of the queue
void enqueue(void *item)
{
    if (queue == NULL)
    {
        fprintf(stderr, "Queue not initialized.\n");
        return;
    }

    // Create a new node for the item
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    if (newNode == NULL)
    {
        perror("Memory allocation error");
        return;
    }
    newNode->data = item;
    newNode->next = NULL;

    mtx_lock(&queue->mutex);

    // Update the tail and head pointers
    if (queue->tail == NULL)
    {
        queue->head = newNode;
    }
    else
    {
        queue->tail->next = newNode;
    }
    queue->tail = newNode;

    // Signal any waiting threads that the queue is no longer empty
    cnd_signal(&queue->notEmpty);
    mtx_unlock(&queue->mutex);
}

// Removes and returns the first item from the queue, blocking if the queue is empty
void *dequeue(void)
{
    if (queue == NULL)
    {
        fprintf(stderr, "Queue not initialized.\n");
        return NULL;
    }

    mtx_lock(&queue->mutex);

    // Wait until the queue has at least one item
    while (queue->head == NULL)
    {
        cnd_wait(&queue->notEmpty, &queue->mutex);
    }

    // Remove the head node and update the queue pointers
    QueueNode *temp = queue->head;
    void *item = temp->data;
    queue->head = queue->head->next;
    if (queue->head == NULL)
    {
        queue->tail = NULL;
    }
    queue->visited++;

    free(temp);
    mtx_unlock(&queue->mutex);

    return item;
}

// Tries to remove an item from the queue without blocking
bool tryDequeue(void **item)
{
    if (queue == NULL)
    {
        fprintf(stderr, "Queue not initialized.\n");
        return false;
    }

    mtx_lock(&queue->mutex);

    // Check if the queue is empty
    if (queue->head == NULL)
    {
        mtx_unlock(&queue->mutex);
        return false;
    }

    // Remove the head node and update the queue pointers
    QueueNode *temp = queue->head;
    *item = temp->data;
    queue->head = queue->head->next;
    if (queue->head == NULL)
    {
        queue->tail = NULL;
    }
    queue->visited++;

    free(temp);
    mtx_unlock(&queue->mutex);

    return true;
}

// Returns the total number of items that have been dequeued
size_t visited(void)
{
    if (queue == NULL)
        return 0;

    mtx_lock(&queue->mutex);
    size_t result = queue->visited;
    mtx_unlock(&queue->mutex);

    return result;
}
